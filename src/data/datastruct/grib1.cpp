#include "data/datastruct/grib1.h"

std::vector<std::pair<path::Storage<false>,std::vector<std::ptrdiff_t>>> Grib1Data::match_files(
    utc_tp last_update,
    Coord pos,
    Organization center,
    const std::unordered_set<SearchParamTableVersion>& param_tables,
    std::optional<utc_tp_t<std::chrono::seconds>> from,
    std::optional<utc_tp_t<std::chrono::seconds>> to,
    std::optional<DateTimeDiff> diff,
    std::optional<TimeForecast> forecast_preferences,
    std::optional<Level> level,
    std::optional<RepresentationType> rep_t
) const{
    std::vector<std::pair<path::Storage<false>,std::vector<std::ptrdiff_t>>> result;
    for(auto [path,idx]:paths_){
        if(path){
            auto loc_res = match(path->path_,last_update,
            pos,center,param_tables,from,to,diff,forecast_preferences,
            level,rep_t);
            if(!loc_res.empty())
                result.push_back(std::make_pair(*path,std::move(loc_res)));
        }
    }
    return result;
}

//match data by specified file
std::vector<ptrdiff_t> Grib1Data::match(
        std::string_view path,
        utc_tp last_update,
        Coord pos,
        Organization center,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        std::optional<utc_tp_t<std::chrono::seconds>> from,
        std::optional<utc_tp_t<std::chrono::seconds>> to,
        std::optional<DateTimeDiff> diff,
        std::optional<TimeForecast> time_fcst,
        std::optional<Level> level,
        std::optional<RepresentationType> rep_t
    ) const
{
    if(parameters.empty())
        return std::vector<ptrdiff_t>();
    auto check_update = [last_update](const auto& add_path){
        using type = std::decay_t<decltype(add_path)>;
        if constexpr(std::is_same_v<type,std::monostate>)
            return false;
        else return add_path.last_check_>=last_update;
    };
    if(auto found_path = paths_.find(path::Storage<true>::file(path));found_path==paths_.end() ||
            !found_path->first ||
            !std::visit(check_update,found_path->first->add_))
        return std::vector<ptrdiff_t>();
    else{
        std::vector<ptrdiff_t> result;
        std::vector<std::weak_ptr<IndexStruct>> ids(found_path->second.begin(),found_path->second.end());
        std::sort(ids.begin(),
                    ids.end(),
                    []( const std::weak_ptr<IndexStruct>& lhs,
                        const std::weak_ptr<IndexStruct>& rhs){
                return  lhs.expired()?false:
                        (rhs.expired()?true:
                        lhs.lock().get()<rhs.lock().get());
        });
        if(ids.empty())
            return result;
        while(ids.back().expired())
            ids.pop_back();
        if(ids.empty())
            return result;
        std::vector<std::weak_ptr<IndexStruct>> ids_tmp;
        ids_tmp.reserve(ids.size());
        if(from.has_value() || to.has_value()){
            TimeInterval from_interval(from.has_value()?*from:utc_tp(),
                                    from.has_value()?*from:utc_tp());
            TimeInterval to_interval(to.has_value()?*to:utc_tp::clock::now(),
                                    to.has_value()?*to:utc_tp::clock::now());
            auto begin_iter = by_intervals_.lower_bound(from_interval);
            auto end_iter = by_intervals_.upper_bound(to_interval);
            if(end_iter==begin_iter)
                return result;
            for(auto interval_iter = begin_iter;interval_iter!=end_iter;++interval_iter){
                for(auto& id:ids)
                    if(interval_iter->second.contains(id))
                        ids_tmp.push_back(id);
            }
            ids_tmp.swap(ids);
            ids_tmp.clear();
            if(ids.empty())
                return result;
        }
        if(diff.has_value()){
            auto begin_iter = by_diff_.lower_bound(*diff);
            auto end_iter = by_diff_.upper_bound(*diff);
            for(auto interval_iter = begin_iter;interval_iter!=end_iter;++interval_iter){
                for(auto& id:ids)
                    if(interval_iter->second.contains(id))
                        ids_tmp.push_back(id);
            }
            ids_tmp.swap(ids);
            ids_tmp.clear();
            if(ids.empty())
                return result;
        }
        if(time_fcst.has_value()){
            if(auto found_fcst = tf_.find(*time_fcst);found_fcst!=tf_.end()){
                for(auto& id:ids)
                    if(found_fcst->second.contains(id))
                        ids_tmp.push_back(id);
                ids_tmp.swap(ids);
                ids_tmp.clear();
                if(ids.empty())
                    return result;
            }
            else return result;
        }
        if(level.has_value()){
            if(auto found_lvl = levels_.find(*level);found_lvl!=levels_.end()){
                for(auto& id:ids)
                    if(found_lvl->second.contains(id))
                        ids_tmp.push_back(id);
                ids_tmp.swap(ids);
                ids_tmp.clear();
                if(ids.empty())
                    return result;
            }
            else return result;
        }
        
        for(auto& [grid,grid_ids]:grids_){
            if(grid && pos_in_grid(pos,*grid) &&
                rep_t.has_value()?grid->type()==rep_t:true){
                for(auto& id:ids)
                    if(grid_ids.contains(id))
                        ids_tmp.push_back(id);
            }
            else continue;
        }
        ids_tmp.swap(ids);
        ids_tmp.clear();
        if(ids.empty())
            return result;

        for(auto& param:parameters){
            CommonDataProperties<Data_t::TIME_SERIES,
                Data_f::GRIB_v1> cmn(center,param.t_ver_,param.param_);
            if(auto found_cmn=common_.find(cmn);found_cmn!=common_.end()){
                for(auto& id:ids){
                    if(!id.expired()){
                        if(auto locked_id = id.lock();!locked_id || locked_id->cmn_!=cmn)
                            continue;
                        else ids_tmp.push_back(id);
                    }
                    else continue;
                }
            }
            else continue;
        }
        ids_tmp.swap(ids);
        ids_tmp.clear();
        if(ids.empty())
            return result;
            
        TimeInterval interval(from.has_value()?*from:utc_tp(),
                        to.has_value()?*to:utc_tp::clock::now());
        for(auto& id:ids){
            auto locked_id = id.lock();
            if(!locked_id)
                continue;
            std::error_code err;
            for(auto& [ts,positions]:locked_id->ts_pos_){
                auto beg_end = interval_intersection_pos(interval,ts,err);
                if(beg_end.has_value() && err==std::error_code())
                    result.append_range(std::move(positions|std::views::drop(beg_end->first)|std::views::take(beg_end->second-beg_end->first+1)));
                else continue;
            }
        }        
        std::sort(result.begin(),result.end());
        return result;
    }
}

std::vector<SearchDataResult<Data_t::TIME_SERIES,
    Data_f::GRIB_v1>> Grib1Data::find_all(
        const std::unordered_set<
                CommonDataProperties<Data_t::TIME_SERIES,
                Data_f::GRIB_v1>>& common,
        utc_tp last_update,
        std::optional<Lat> top,
        std::optional<Lat> bottom,
        std::optional<Lon> left,
        std::optional<Lon> right,
        std::optional<utc_tp_t<std::chrono::seconds>> from,
        std::optional<utc_tp_t<std::chrono::seconds>> to,
        std::optional<DateTimeDiff> tdiff,
        std::optional<std::pair<TimeForecast,TimeForecast::COMPARISION_TYPE>> forecast_preference,
        std::optional<std::pair<Level,Level::COMPARISION_TYPE>> level,
        std::optional<RepresentationType> grid_type) const
{
    std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>> result;
    std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal> idx_tmp;
    std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal> current;
    auto check_update = [last_update](const auto& add_path){
        using type = std::decay_t<decltype(add_path)>;
        if constexpr(std::is_same_v<type,std::monostate>)
            return false;
        else return add_path.last_check_>=last_update;
    };
    for(auto& [path,idx]:paths_){
        if(path){
            if(!std::visit(check_update,path->add_))
                continue;
            for(auto& id:idx)
                if(!id.expired())
                    idx_tmp.insert(id);
        }
        else continue;
    }
    if(idx_tmp.empty())
        return std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>();
    if(!common.empty()){
        for(auto& cmn_tmp:common){
            if(auto found_cmn = common_.find(cmn_tmp);found_cmn!=common_.end()){
                for(auto id:found_cmn->second){
                    if(!id.expired() && idx_tmp.contains(id))
                        current.insert(id);
                    else continue;
                }
            }
            else continue;
        }
        current.swap(idx_tmp);
        current.clear();
        if(idx_tmp.empty())
            return std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>();
    }
    if(top.value() || bottom.value() || left.value() || right.value()){
        for(auto& [grid,idx]:grids_){
            if(grid){
                if(grid_type.has_value() && grid->type()!=*grid_type)
                    continue;
                if(top.has_value())
                    if(auto top_tmp = grid->top();top_tmp.has_value() && top.value()>top_tmp.value())
                        continue;
                if(bottom.has_value())
                    if(auto bottom_tmp = grid->bottom();bottom_tmp.has_value() && bottom.value()<bottom_tmp.value())
                        continue;
                if(left.has_value())
                    if(auto left_tmp = grid->left();left_tmp.has_value() && left.value()<left_tmp.value())
                        continue;
                if(right.has_value())
                    if(auto right_tmp = grid->right();right_tmp.has_value() && right.value()>right_tmp.value())
                        continue;
                for(auto id:idx){
                    if(!id.expired() && idx_tmp.contains(id))
                        current.insert(id);
                    else continue;
                }
            }
        }
        current.swap(idx_tmp);
        current.clear();
        if(idx_tmp.empty())
            return std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>();
    }
    if(level){
        for(auto [lvl,idx]:levels_){
            std::error_code err;
            if(bool is = Level::compare(
                        level->second,lvl,level->first,err);
                    err==std::error_code() && is){
                for(auto id:idx)
                    if(!id.expired() && 
                        idx_tmp.contains(id))
                        current.insert(id);
                    else continue;
            }
            else continue;
        }
        current.swap(idx_tmp);
        current.clear();
        if(idx_tmp.empty())
            return std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>();
    }
    if(forecast_preference){
        for(auto [tf,idx]:tf_){
            std::error_code err;
            if(bool is = TimeForecast::compare(
                        forecast_preference->second,tf,forecast_preference->first,err);
                    err==std::error_code() && is){
                for(auto id:idx)
                    if(!id.expired() && 
                        idx_tmp.contains(id))
                        current.insert(id);
                    else continue;
            }
            else continue;
        }
        current.swap(idx_tmp);
        current.clear();
        if(idx_tmp.empty())
            return std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>();
    }
    std::cout<<"bounding interval:"<<"\n"<<
    "from:"<<(from.has_value()?*from:utc_tp())<<"\n"<<
    "to"<<(to.has_value()?*to:utc_tp::clock::now())<<std::endl;

    for(auto& idx:idx_tmp){
        auto idx_lock = idx.lock();
        for(auto& [ts,pos]:idx_lock->ts_pos_){
            SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1> to_add;
            to_add.cmn_=idx_lock->cmn_;
            to_add.add_.fcst_=idx_lock->tf_;
            to_add.add_.lvl_=idx_lock->lvl_;
            to_add.add_.grid_=idx_lock->grid_.lock();
            if(from.has_value() || to.has_value()){
                TimeInterval tinterval(from.has_value()?*from:utc_tp(),
                                to.has_value()?*to:utc_tp::clock::now());
                std::cout<<"current interval:"<<"\n"<<
                "from:"<<ts.get_interval().from()<<"\n"<<
                "to"<<ts.get_interval().to()<<std::endl;
                if(!intervals_intersect(ts.get_interval(),tinterval))
                    continue;
            }
            if(tdiff.has_value() && ts.time_duration()>*tdiff)
                continue;
            std::error_code err;
            auto ts_tmp = ts.bound_by_interval(TimeInterval(from.has_value()?*from:utc_tp(),
                                to.has_value()?*to:utc_tp::clock::now()),err);
            if(err!=std::error_code())
                continue;
            to_add.add_.ts_=ts_tmp;
            result.push_back(to_add);
        }
    }
    return result;
}

void DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::rewrite_index(const std::shared_ptr<IndexStruct>& data){
    if(data){
        if(auto found = index_.find(*data);found!=index_.end())
            (*found)->ts_pos_.swap(data->ts_pos_);
        else{
            if(!data->grid_.expired() && !data->path_.expired()){
                if(auto found_path = paths_.find(*data->path_.lock());
                    found_path!=paths_.end()){
                    data->path_=found_path->first;
                    found_path->second.insert(data);
                }
                else
                {
                    auto iter = paths_.insert(std::make_pair(
                        data->path_.lock(),
                        std::unordered_set<std::weak_ptr<IndexStruct>,
                        IndexStruct::Hash,IndexStruct::Equal>())).first;
                    iter->second.insert(data);
                    data->path_ = iter->first;
                }
                if(auto found_grid = grids_.find(*data->grid_.lock());
                    found_grid!=grids_.end()){
                    data->grid_=found_grid->first;
                    found_grid->second.insert(data);
                }
                else{
                    auto iter_grid = grids_.insert(std::make_pair(
                        data->grid_.lock(),
                    std::unordered_set<std::weak_ptr<IndexStruct>,
                    IndexStruct::Hash,IndexStruct::Equal>())).first;
                    iter_grid->second.insert(data);
                    data->grid_=iter_grid->first;
                }
                if(auto found_common = common_.find(data->cmn_);
                found_common!=common_.end()){
                    data->cmn_=found_common->first;
                    found_common->second.insert(data);
                }
                else{
                    auto iter_common = common_.insert(
                        std::make_pair(data->cmn_,
                        std::unordered_set<std::weak_ptr<IndexStruct>,
                        IndexStruct::Hash,IndexStruct::Equal>())).first;
                    iter_common->second.insert(data);
                    data->cmn_=iter_common->first;
                }
                if(auto found_lvl = levels_.find(data->lvl_);
                    found_lvl!=levels_.end()){
                        data->lvl_=found_lvl->first;
                        found_lvl->second.insert(data);
                }
                else{
                    auto iter_lvl = levels_.insert(std::make_pair(
                        data->lvl_,
                        std::unordered_set<std::weak_ptr<IndexStruct>,
                        IndexStruct::Hash,IndexStruct::Equal>())).first;
                            iter_lvl->second.insert(data);
                            data->lvl_=iter_lvl->first;
                }
                if(auto found_tf = tf_.find(data->tf_);
                    found_tf!=tf_.end()){
                        data->tf_=found_tf->first;
                        found_tf->second.insert(data);
                }
                else{
                    auto iter_tf = tf_.insert(std::make_pair(data->tf_,
                        std::unordered_set<std::weak_ptr<IndexStruct>,
                        IndexStruct::Hash,IndexStruct::Equal>())).first;
                            iter_tf->second.insert(data);
                            data->tf_=iter_tf->first;
                }
                for(auto& [ts,pos]:data->ts_pos_){
                    by_intervals_[ts.get_interval()].insert(data);
                    by_diff_[ts.time_duration()].insert(data);
                }
                if(!index_.contains(*data))
                    index_.insert(data);
            }
        }
    }
}

void DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::add_data(const path::Storage<false>& path,
        const std::vector<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>& grib_msg,
        std::error_code& err)
{   
    if(!std::is_sorted(grib_msg.begin(),grib_msg.end(),[]
    (const FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>& lhs,
        const FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>& rhs)
        {
            return lhs.date<rhs.date;
        }))
    {
        err = std::make_error_code(std::errc::invalid_argument);
        return;
    }
    
    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1> tmp;
    for(auto& msg:grib_msg)
    {
        if(msg.err_!=decltype(msg.err_)::NONE_ERR){
            err=std::make_error_code(std::errc::bad_message);
            return;
        }
        std::shared_ptr<IndexStruct> idx_tmp = std::make_shared<IndexStruct>();
        idx_tmp->cmn_=Grib1CommonDataProperties(
                msg.center,msg.table_version,msg.parameter);
        idx_tmp->lvl_=msg.level_;
        idx_tmp->tf_=msg.t_unit;
        std::shared_ptr<GridInfo> grid_tmp;
        if(auto found_grid =tmp.grids_.find(*msg.grid_data);
            found_grid!=tmp.grids_.end()){
                grid_tmp = found_grid->first;
                idx_tmp->grid_=grid_tmp;
        }
        else{
            grid_tmp = msg.grid_data;
            idx_tmp->grid_ = grid_tmp;
        }
        std::shared_ptr<path::Storage<false>> file;
        if(auto found_path = tmp.paths_.find(path);found_path!=tmp.paths_.end())
            idx_tmp->path_=found_path->first;
        else{
            file = std::make_shared<path::Storage<false>>(path);
            idx_tmp->path_ = file;
        }
        if(auto found = tmp.index_.find(*idx_tmp);found!=tmp.index_.end()){
            auto iter = (*found)->ts_pos_.begin();
            auto tinterval_tmp = iter->first.get_interval();
            DateTimeDiff diff_tmp = iter->first.time_duration();
            //trying push time_point to TimeSequence
            while(iter!=(*found)->ts_pos_.end() &&
                    !iter->first.push_time_after(msg.date,err) &&
                    err!=std::error_code() &&
                    !iter->first.push_time_before(msg.date,err) &&
                    err!=std::error_code()){
                ++iter;
                if(iter!=(*found)->ts_pos_.end()){
                    tinterval_tmp = iter->first.get_interval();
                    DateTimeDiff diff_tmp = iter->first.time_duration();
                }
            }
            //if not found pushing back in ts_pos a new pair of TimeSequence and GribMsg position
            if(iter==(*found)->ts_pos_.end()){
                (*found)->ts_pos_.push_back(std::make_pair<TimeSequence,
                    std::deque<ptrdiff_t>>(std::move(TimeSequence(msg.date)),
                    {msg.buf_pos_}));
                tmp.by_intervals_[(*found)->ts_pos_.back().first.get_interval()].insert(*found);
            }
            //else firstly remove existing IndexStruct with previous TimeInterval from by_interval
            //then pushing time_point to TimeSequence
            //then search or create new TimeInterval from current IndexStruct TimeSequence with adding
            //found IndexStruct to the set
            else{
                if(auto found_interval=tmp.by_intervals_.find(tinterval_tmp);found_interval!=by_intervals_.end()){
                    if(found_interval->second.contains(*found))
                        found_interval->second.erase(*found);
                    if(found_interval->second.empty())
                        tmp.by_intervals_.erase(found_interval);
                }
                tmp.by_intervals_[iter->first.get_interval()].insert(*found);
                if(diff_tmp==DateTimeDiff()){
                    if(auto found_diff=tmp.by_diff_.find(diff_tmp);found_diff!=tmp.by_diff_.end()){
                        if(found_diff->second.contains(*found))
                            found_diff->second.erase(*found);
                        if(found_diff->second.empty())
                            tmp.by_diff_.erase(found_diff);
                    }
                    tmp.by_diff_[iter->first.time_duration()].insert(*found);
                }
                iter->second.push_back(msg.buf_pos_);
            }
        }
        else{
            idx_tmp->ts_pos_.push_back(std::make_pair<TimeSequence,
                std::deque<ptrdiff_t>>(TimeSequence(msg.date),
                {msg.buf_pos_}));
            tmp.rewrite_index(idx_tmp);
        }
    }
    update_indexing(tmp);
}

void DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::add_data(const path::Storage<false>& path,
    const DataStruct<Data_t::TIME_SERIES,
    Data_f::GRIB_v1>::find_all_t& data){
        auto file = std::make_shared<path::Storage<false>>(path);
        for(const auto& found_data:data){
            std::shared_ptr<IndexStruct> id = 
                std::make_shared<IndexStruct>();
            id->cmn_=found_data.cmn_;
            id->grid_=found_data.add_.grid_;
            id->lvl_=found_data.add_.lvl_;
            id->path_=file;
            id->tf_=found_data.add_.fcst_;
            id->ts_pos_.push_back(std::make_pair(found_data.add_.ts_,
                std::deque<ptrdiff_t>()));
            rewrite_index(id);
        }
}

bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::operator==(const IndexStruct& other) const{
        auto grid1 = grid_.lock();
        auto grid2 = other.grid_.lock();
        auto path1 = path_.lock();
        auto path2 = other.path_.lock();
        if (!grid1 || !grid2) {
            if ((!grid1 && grid2) || (grid1 && !grid2))
                return false;
        } else if (*grid1 != *grid2) {
            return false;
        }
        if (!path1 || !path2) {
            if ((!path1 && path2) || (path1 && !path2))
                return false;
        } else if (path1->path_ != path2->path_) {
            return false;
        }
        return tf_ == other.tf_ &&
               cmn_ == other.cmn_ &&
               lvl_ == other.lvl_;
    }
void DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    delete_index(const path::Storage<false>& path){
    if(auto found = paths_.find(path);found!=paths_.end()){
        std::unordered_set<std::shared_ptr<IndexStruct>,
            IndexStruct::Hash,IndexStruct::Equal> removed;
        for(auto& index:found->second){
            if(index.expired())
                continue;
            else{
                const auto& cur_index = *removed.insert(
                    index.lock()).first;
                common_.find(cur_index->cmn_)->second.erase(cur_index);
                grids_.find(cur_index->grid_)->second.erase(cur_index);
                levels_.find(cur_index->lvl_)->second.erase(cur_index);
                tf_.find(cur_index->tf_)->second.erase(cur_index);
                tf_.find(cur_index->tf_)->second.erase(cur_index);
            }
        }
        paths_.erase(paths_.find(path));
        for(const auto& id_remove:removed)
            index_.erase(id_remove);
    }
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    operator==(const DataStruct& other) const{
    if(by_diff_.size()!=other.by_diff_.size())
        return false;
    if(by_intervals_.size()!=other.by_intervals_.size())
        return false;
    if(common_.size()!=other.common_.size())
        return false;
    if(grids_.size()!=other.grids_.size())
        return false;
    if(index_.size()!=other.index_.size())
        return false;
    if(levels_.size()!=other.levels_.size())
        return false;
    for(auto& [diff,ids]:by_diff_){
        if(auto found = other.by_diff_.find(diff);found!=
            other.by_diff_.end()){
                if(found->second.size()!=ids.size())
                    return false;
                for(auto& id:ids)
                    if(found->second.find(id)==found->second.end())
                        return false;
        }
        else return false;
    }
    for(auto& [interval,ids]:by_intervals_){
        if(auto found = other.by_intervals_.find(interval);
            found!=other.by_intervals_.end()){
                if(found->second.size()!=ids.size())
                    return false;
                for(auto& id:ids)
                    if(found->second.find(id)==found->second.end())
                        return false;
        }
        else return false;
    }
    for(auto& [cmn,ids]:common_){
        if(auto found = other.common_.find(cmn);
            found!=other.common_.end()){
                if(found->second.size()!=ids.size())
                    return false;
                for(auto& id:ids)
                    if(found->second.find(id)==found->second.end())
                        return false;
        }
        else return false;
    }
    for(auto& [grid,ids]:grids_){
        if(auto found = other.grids_.find(grid);
            found!=other.grids_.end()){
                if(found->second.size()!=ids.size())
                    return false;
                for(auto& id:ids)
                    if(found->second.find(id)==found->second.end())
                        return false;
        }
        else return false;
    }
    for(auto& id:index_){
        if(other.index_.find(id)==other.index_.end())
            return false;
    }
    for(auto& [lvl,ids]:levels_){
        if(auto found = levels_.find(lvl);found!=other.levels_.end()){
            if(found->second.size()!=ids.size())
                return false;
            for(auto& id:ids)
                if(found->second.find(id)==found->second.end())
                    return false;
        }
        else return false;
    }
    return true;
}

bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::operator!=(const IndexStruct& other) const{
    return !(*this==other);
}

size_t DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Hash::operator()(const IndexStruct& val) const{
    size_t hash = 0;
    using namespace boost;
    auto path_lock = val.path_.lock();
    auto grid_lock = val.grid_.lock();
    if(!path_lock)
        return 0;
    else
        hash_combine(hash,std::hash<std::string_view>()(
            path_lock->path_));
    if(!grid_lock)
        return 0;
    else
        hash_combine(hash,std::hash<GridInfo>()(*grid_lock));
    hash_combine(hash,std::hash<TimeForecast>()(val.tf_));
    hash_combine(hash,std::hash<Grib1CommonDataProperties>()(
            val.cmn_));
    hash_combine(hash,std::hash<Level>()(val.lvl_));
    return hash;
}

size_t DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Hash::operator()(const std::weak_ptr<IndexStruct>& val) const{
    return this->operator()(val.lock());
}
size_t DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Hash::operator()(const std::shared_ptr<IndexStruct>& val) const{
    if(!val)
        return 0;
    else return this->operator()(*val);
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const IndexStruct& lhs,
        const IndexStruct& rhs) const{
    return lhs==rhs;
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const std::shared_ptr<IndexStruct>& lhs,
        const IndexStruct& rhs) const{
    if(lhs)
        return this->operator()(*lhs,rhs);
    else return false;
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const IndexStruct& lhs,
        const std::shared_ptr<IndexStruct>& rhs) const{
    return operator()(rhs,lhs);
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const std::weak_ptr<IndexStruct>& lhs,
        const IndexStruct& rhs) const{
    if(lhs.expired()) return false;
    else return operator()(lhs.lock(),rhs);
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const IndexStruct& lhs,
        const std::weak_ptr<IndexStruct>& rhs) const{
    return operator()(rhs,lhs);
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const std::shared_ptr<IndexStruct>& lhs,
        const std::shared_ptr<IndexStruct>& rhs) const{
    if(rhs && lhs) return operator()(*lhs,*rhs);
    else return false;
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const std::weak_ptr<IndexStruct>& lhs,
        const std::shared_ptr<IndexStruct>& rhs) const{
    if(lhs.expired()) return false;
    else return operator()(lhs.lock(),rhs);
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const std::shared_ptr<IndexStruct>& lhs,
        const std::weak_ptr<IndexStruct>& rhs) const{
    return operator()(rhs,lhs);
}
bool DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
        IndexStruct::Equal::operator()(const std::weak_ptr<IndexStruct>& lhs,
        const std::weak_ptr<IndexStruct>& rhs) const{
    if(lhs.expired()||rhs.expired())return false;
    return operator()(lhs.lock(),rhs.lock());
}