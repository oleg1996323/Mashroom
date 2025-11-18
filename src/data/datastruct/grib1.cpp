#include "data/datastruct/grib1.h"

void Grib1Data::add_data(SublimedGribDataInfo& grib_data){
    if(grib_data.data().empty()){
        std::cout<<"Nothing changes"<<std::endl;
        return;
    }
    sublimed_.add_data(grib_data);
    for(auto& [filename,file_data]:grib_data.data()){
        const auto& tmp_view = sublimed_.data().find(filename)->first;
        for(auto& [common,grib_data]:sublimed_.data().find(filename)->second){
            by_common_data_[common].insert(tmp_view);
            for(const auto& sub_data:grib_data){
                by_date_[sub_data.sequence_time_].insert(tmp_view);
                by_grid_[sub_data.grid_data_].insert(tmp_view);
            }
        }
    }
}
void Grib1Data::add_data(SublimedGribDataInfo&& grib_data){
    if(grib_data.data().empty()){
        std::cout<<"Nothing changes"<<std::endl;
        return;
    }
    sublimed_.add_data(grib_data);
    for(auto& [filename,file_data]:grib_data.data()){
        const auto& tmp_view = sublimed_.data().find(filename)->first;
        for(auto& [common,grib_data]:sublimed_.data().find(filename)->second){
            by_common_data_[common].insert(tmp_view);
            for(const auto& sub_data:grib_data){
                by_date_[sub_data.sequence_time_].insert(tmp_view);
                by_grid_[sub_data.grid_data_].insert(tmp_view);
            }
        }
    }
}

//match data and return files and corresponding sublimed data
std::unordered_map<path::Storage<true>,GribSublimedDataInfoStruct> Grib1Data::match_data(
    Organization center,
    std::optional<TimeFrame> time_fcst,
    const std::unordered_set<SearchParamTableVersion>& parameters,
    TimeInterval time_interval,
    RepresentationType rep_t,
    Coord pos
) const{
    std::unordered_map<path::Storage<true>,GribSublimedDataInfoStruct> result;
    std::unordered_set<Grib1CommonDataProperties> param_variations=get_parameter_variations(center,time_fcst,parameters);
    for(const auto& common:param_variations){
        auto found = by_common_data_.find(common);
        if(found!=by_common_data_.end() && !found->first.expired()){
            for(auto fn:found->second)
                result[fn];
        }
    }
    for(const auto& [date_interval,seq_fn]:by_date_){
        if(!intervals_intersect(time_interval,date_interval.get_interval()))
            for(auto fn:seq_fn){
                result.erase(fn);
            }
    }
    for(const auto& [grid,seq_fn]:by_grid_){
        if(!grid.has_value())
            for(auto fn:seq_fn){
                result.erase(fn);
            }
        if(!pos_in_grid(pos,grid.value()))
            for(auto fn:seq_fn){
                result.erase(fn);
            }
    }
    for(auto& [fn,ptrs]:result){
        std::cout<<fn<<" parameters:"<<std::flush;
        for(const auto& [common,data]:sublimed_.data().find(fn)->second)
            if(param_variations.contains(*common))
                for(const auto& d:data){
                    if(d.grid_data_.has_value() && 
                        d.grid_data_.value().type()==rep_t && 
                        intervals_intersect(d.sequence_time_.get_interval().from(),d.sequence_time_.get_interval().to(),time_interval.from(),time_interval.to()) && 
                        pos_in_grid(pos,d.grid_data_.value()))
                    {
                        std::cout<<" "<<std::string_view(parameter_table(common->center_.value(),common->table_version_.value(),common->parameter_.value())->name)<<std::flush;
                        auto beg_end = interval_intersection_pos(time_interval,TimeSequence(d.sequence_time_.get_interval().from(),d.sequence_time_.get_interval().to(),d.sequence_time_.discret()));
                        if(beg_end.has_value())
                            ptrs.buf_pos_.append_range(d.buf_pos_|std::views::drop(beg_end->first)|std::views::take(beg_end->second-beg_end->first+1));
                        auto interseq_interval = interval_intersection(d.sequence_time_.get_interval(),time_interval);
                        assert(interseq_interval.has_value());
                        ptrs.sequence_time_ = std::move(TimeSequence(interseq_interval->from(),interseq_interval->to(),d.sequence_time_.discret()));
                        ptrs.grid_data_ = d.grid_data_;
                    }
                }
        std::cout<<std::endl;
    }
    return result;
}

//match data by specified file
std::vector<ptrdiff_t> Grib1Data::match(
        path::Storage<true> path,
        Organization center,
        std::optional<TimeFrame> time_fcst,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        TimeInterval time_interval,
        RepresentationType rep_t,
        Coord pos
    ) const
{
    std::vector<ptrdiff_t> result;
    std::unordered_set<Grib1CommonDataProperties> param_variations=get_parameter_variations(center,time_fcst,parameters);
    if(!sublimed_.data().contains(path))
        return result;
    auto& file_data = sublimed_.data().at(path);
    for(const auto& common:param_variations){
        auto found = file_data.find(common);
        if(found!=file_data.end()){
            for(auto info:found->second){
                if(info.grid_data_.has_value() &&
                    info.grid_data_.value().type()==rep_t &&
                    pos_in_grid(pos,info.grid_data_.value()) &&
                    intervals_intersect(info.sequence_time_.get_interval(),time_interval))
                {
                    auto beg_end = interval_intersection_pos(time_interval,TimeSequence(info.sequence_time_.get_interval().from(),info.sequence_time_.get_interval().to(),info.sequence_time_.discret()));
                    if(beg_end.has_value())
                        result.append_range(std::move(info.buf_pos_|std::views::drop(beg_end->first)|std::views::take(beg_end->second-beg_end->first+1)));
                }
            }
        }
    }
    std::sort(result.begin(),result.end());
    return result;
}

FoundSublimedDataInfo<Data_t::METEO,Data_f::GRIB_v1> Grib1Data::find_all(std::optional<RepresentationType> grid_type_,
                std::optional<TimeSequence> time_,
                std::optional<TimeFrame> forecast_preference_,
                utc_tp last_update_) const{
    FoundSublimedDataInfo<Data_t::METEO,Data_f::GRIB_v1> result;
    for(const auto& [path,dat]:this->sublimed_.data()){
        if(path.type_==path::TYPE::FILE && path.add_.get<path::TYPE::FILE>().last_check_>=last_update_){
            Grib1CommonDataProperties searched(std::nullopt,std::nullopt,forecast_preference_,std::nullopt);
            auto filtered = std::views::filter(dat,[&](const std::decay_t<decltype(dat)>::value_type& cmn){
                return cmn.first && forecast_preference_?cmn.first->fcst_unit_==forecast_preference_:true;
            });
            for(const auto& [cmn,sublimed_deque]:filtered){
                if(time_){
                    for(auto& sub:sublimed_deque){
                        if( auto intersection = interval_intersection(time_->get_interval(),sub.sequence_time_.get_interval());
                            intersection.has_value())
                            result[*cmn].push_back(sub);
                        else continue;
                    }
                }
                else {
                    auto& sub_deq_res = result[*cmn];
                    sub_deq_res.resize(sub_deq_res.size()+sublimed_deque.size());
                    for(int id = 0;id<sublimed_deque.size();++id)
                        sub_deq_res[sub_deq_res.size()-sublimed_deque.size()+id]=sublimed_deque[id];
                }
            }
        }
        else continue;
    }
    return result;
}

std::unordered_set<Grib1CommonDataProperties> Grib1Data::get_parameter_variations(Organization center,
    std::optional<TimeFrame> time_fcst,
    const std::unordered_set<SearchParamTableVersion>& parameters) noexcept
{
    std::unordered_set<Grib1CommonDataProperties> result;
    if(!time_fcst.has_value()){
        int time_counter = 0;
        for(int time=0;time<256;++time)
            if(is_time[time]){
                if(parameters.empty()){
                    for(int param=0;param<256;++param){
                        for(int table_version=0;table_version<256;++table_version)
                            result.insert(Grib1CommonDataProperties(center,table_version,(TimeFrame)time,param));
                    }
                }
                else{
                    int param_counter = 0;
                    for(const auto& [param,table_version]:parameters)
                        result.insert(Grib1CommonDataProperties(center,table_version,(TimeFrame)time,param));
                }
                ++time_counter;
            }
            else continue;
    }
    else{
            if(is_time[time_fcst.value()] && parameters.empty()){
                for(uint8_t param=0;param<256;++param)
                    for(int table_version=0;table_version<256;++table_version)
                        result.insert(Grib1CommonDataProperties(center,table_version,time_fcst.value(),param));
            }
            else{
                uint8_t param_counter = 0;
                for(const auto& [param,table_version]:parameters)
                    result.insert(Grib1CommonDataProperties(center,table_version,time_fcst.value(),param));
            }
    }
    return result;
}