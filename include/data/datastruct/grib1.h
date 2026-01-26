#pragma once
#include "searchdataresult.h"
#include "data/msg.h"
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include "data/common_data_properties.h"
#include "API/grib1/include/sections/product/levels.h"
#include "API/grib1/include/sections/product/time_forecast.h"
#include "API/grib1/include/sections/grid/grid.h"
#include "types/time_interval.h"
#include <boost/functional/hash.hpp>
#include "proc/index/index_result.h"

using Grib1Data = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

template<>
struct DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
    using sublimed_data_by_common_data = std::unordered_map<std::shared_ptr<Grib1CommonDataProperties>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>>;
    using sublimed_data_by_date_time = std::map<std::shared_ptr<TimeSequence>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>>;
    using sublimed_data_by_grid = std::unordered_map<std::shared_ptr<GridInfo>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>>;
    using sublimed_data_by_forecast = std::unordered_map<std::shared_ptr<TimeForecast>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>>;
    using sublimed_data_by_level = std::unordered_map<std::shared_ptr<Level>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>>;

    // struct path_props{
    //     std::vector<ptrdiff_t> positions_;
    //     std::shared_ptr<Grib1CommonDataProperties> cmn_;
    //     std::shared_ptr<GridInfo> grid_;
    //     std::shared_ptr<TimeForecast> fcst_;
    //     std::shared_ptr<Level> lvl_;
    // };
    struct IndexStruct{
        std::weak_ptr<GridInfo> grid_;
        std::weak_ptr<path::Storage<false>> path_;
        std::vector<std::pair<TimeSequence,std::vector<ptrdiff_t>>> ts_pos_;
        TimeForecast tf_;
        Grib1CommonDataProperties cmn_;
        Level lvl_;
        constexpr Data_t type() const noexcept{
            return Data_t::TIME_SERIES;
        }
        constexpr Data_f format() const noexcept{
            return Data_f::GRIB_v1;
        }
        struct Hash{
            using is_transparent = std::true_type;
            size_t operator()(const IndexStruct& val) const{
                size_t hash = 0;
                using namespace boost;
                if(val.path_.expired())
                    return 0;
                else
                    hash_combine(hash,std::hash<std::string_view>()(val.path_.lock()->path_));
                if(val.grid_.expired())
                    return 0;
                else
                    hash_combine(hash,std::hash<GridInfo>()(*val.grid_.lock()));
                hash_combine(hash,std::hash<TimeForecast>()(val.tf_));
                hash_combine(hash,std::hash<Grib1CommonDataProperties>()(val.cmn_));
                hash_combine(hash,std::hash<Level>()(val.lvl_));
                return hash;
            }

            size_t operator()(const std::weak_ptr<IndexStruct>& val) const{
                if(val.expired())
                    return 0;
                else return this->operator()(val.lock());
            }
            size_t operator()(const std::shared_ptr<IndexStruct>& val) const{
                if(val)
                    return 0;
                else return this->operator()(*val);
            }
        };

        struct Equal{
            using is_transparent = std::true_type;
            bool operator()(const IndexStruct& lhs,const IndexStruct& rhs) const{
                if(lhs.grid_.expired() || rhs.grid_.expired() || lhs.path_.expired() || rhs.path_.expired())
                    return 0;
                else
                    return *lhs.grid_.lock()==*rhs.grid_.lock()&&lhs.path_.lock()->path_==rhs.path_.lock()->path_&&
                    lhs.tf_==rhs.tf_&& lhs.cmn_==rhs.cmn_&& lhs.lvl_==rhs.lvl_;
            }
            bool operator()(const std::shared_ptr<IndexStruct>& lhs,const IndexStruct& rhs) const{
                if(lhs)
                    return this->operator()(*lhs,rhs);
                else return false;
            }
            bool operator()(const IndexStruct& lhs,const std::shared_ptr<IndexStruct>& rhs) const{
                return operator()(rhs,lhs);
            }
            bool operator()(const std::weak_ptr<IndexStruct>& lhs,const IndexStruct& rhs) const{
                if(lhs.expired()) return false;
                else return operator()(lhs.lock(),rhs);
            }
            bool operator()(const IndexStruct& lhs,const std::weak_ptr<IndexStruct>& rhs) const{
                return operator()(rhs,lhs);
            }
            bool operator()(const std::shared_ptr<IndexStruct>& lhs,const std::shared_ptr<IndexStruct>& rhs) const{
                if(rhs) return operator()(lhs,*rhs);
                else return false;
            }
            bool operator()(const std::weak_ptr<IndexStruct>& lhs,const std::shared_ptr<IndexStruct>& rhs) const{
                if(lhs.expired()) return false;
                else return operator()(lhs.lock(),rhs);
            }
            bool operator()(const std::shared_ptr<IndexStruct>& lhs,const std::weak_ptr<IndexStruct>& rhs) const{
                return operator()(rhs,lhs);
            }
            bool operator()(const std::weak_ptr<IndexStruct>& lhs,const std::weak_ptr<IndexStruct>& rhs) const{
                if(lhs.expired()||rhs.expired())return false;
                return operator()(rhs,lhs);
            }
        };

        IndexStruct() = default;
        IndexStruct(const IndexStruct& other):
        grid_(other.grid_),path_(other.path_),ts_pos_(other.ts_pos_),tf_(other.tf_),cmn_(other.cmn_),lvl_(other.lvl_){}
        IndexStruct(IndexStruct&& other):
        grid_(std::move(other.grid_)),path_(std::move(other.path_)),ts_pos_(std::move(other.ts_pos_)),tf_(other.tf_),cmn_(other.cmn_),lvl_(other.lvl_){}
    };
    // std::unordered_map<path::Storage<false>,std::vector<ptrdiff_t>> positions_;
    // sublimed_data_by_common_data by_common_data_;
    // sublimed_data_by_date_time by_date_;
    // sublimed_data_by_grid by_grid_;
    // sublimed_data_by_forecast by_forecast_;
    // sublimed_data_by_level by_level_;
    
    std::unordered_map<std::shared_ptr<path::Storage<false>>,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>> paths_;
    std::unordered_map<std::shared_ptr<GridInfo>,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>> grids_;
    std::unordered_map<Level,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>> levels_;
    std::unordered_map<TimeForecast,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>> tf_;
    std::unordered_map<Grib1CommonDataProperties,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>> common_;
    std::map<TimeInterval,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>> by_intervals_;
    std::map<DateTimeDiff,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>> by_diff_;
    std::unordered_set<std::shared_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal> index_;

    DataStruct() = default;
    DataStruct(const DataStruct&) = delete;
    DataStruct(DataStruct&& other):
    index_(std::move(other.index_)){}

    std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>> match_data(
        Organization center,
        std::optional<TimeForecast> time_fcst,
        std::optional<Level> level_,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        TimeInterval time_interval,
        RepresentationType rep_t,
        Coord pos
    ) const;

    std::vector<ptrdiff_t> match(
        path::Storage<true> path,
        Organization center,
        std::optional<TimeForecast> time_fcst,
        std::optional<Level> level_,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        TimeInterval time_interval,
        RepresentationType rep_t,
        Coord pos
    ) const;

    std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>> find_all(std::optional<RepresentationType> grid_type_,
                        std::optional<TimeInterval> tinterval,
                        std::optional<DateTimeDiff> tdiff,
                        std::optional<TimeForecast> forecast_preference_,
                        std::optional<Level> level_,
                        std::optional<Lat> top,
                        std::optional<Lat> bottom,
                        std::optional<Lon> left,
                        std::optional<Lon> right,
                        const std::unordered_set<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>& common_,
                        utc_tp last_update_) const;

    static std::unordered_set<Grib1CommonDataProperties> get_parameter_variations(Organization center,
    std::optional<TimeForecast> time_fcst,
    std::optional<Level> level_,
    const std::unordered_set<SearchParamTableVersion>& parameters) noexcept;

    using match_data_t = std::invoke_result_t<  decltype(&DataStruct::match_data),
                                                    DataStruct*,
                                                    Organization,
                                                    std::optional<TimeForecast>,
                                                    std::optional<Level>,
                                                    const std::unordered_set<SearchParamTableVersion>&,
                                                    TimeInterval,
                                                    RepresentationType,
                                                    Coord>;
    using match_t = std::invoke_result_t<           decltype(&DataStruct::match),
                                                    DataStruct*,
                                                    path::Storage<true>,
                                                    Organization,
                                                    std::optional<TimeForecast>,
                                                    std::optional<Level>,
                                                    const std::unordered_set<SearchParamTableVersion>&,
                                                    TimeInterval,
                                                    RepresentationType,
                                                    Coord>;
    using find_all_t = std::invoke_result_t<      decltype(&DataStruct::find_all),
                                                    DataStruct*,
                                                    std::optional<RepresentationType>,
                                                    std::optional<TimeInterval>,
                                                    std::optional<DateTimeDiff>,
                                                    std::optional<TimeForecast>,
                                                    std::optional<Level>,
                                                    std::optional<Lat>,
                                                    std::optional<Lat>,
                                                    std::optional<Lon>,
                                                    std::optional<Lon>,
                                                    const std::unordered_set<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>&,
                                                    utc_tp>;

    void delete_index(const path::Storage<false>& path){
        if(auto found = paths_.find(path);found!=paths_.end()){
            std::unordered_set<std::shared_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal> removed;
            for(auto& index:found->second){
                if(index.expired())
                    continue;
                else{
                    const auto& cur_index = *removed.insert(index.lock()).first;
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

    void add_data(const std::shared_ptr<IndexStruct>& data){
        if(data){
            if(auto found = index_.find(data);found!=index_.end())
                (*found)->ts_pos_.swap(data->ts_pos_);
            else{
                if(!data->grid_.expired() && !data->path_.expired()){
                    std::shared_ptr<IndexStruct> index = std::make_shared<IndexStruct>(*data);
                    std::shared_ptr<path::Storage<false>> path;
                    if(auto found_path = paths_.find(*index->path_.lock());found_path!=paths_.end()){
                        *path=*index->path_.lock();
                        index->path_=path;
                    }
                    else
                    {
                        auto iter = paths_.insert(std::make_pair(std::make_shared<path::Storage<false>>(*index->path_.lock()),
                                std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>())).first;
                        iter->second.insert(index);
                        index->path_ = iter->first;
                    }
                    
                    if(auto found_grid = grids_.find(*index->grid_.lock());found_grid!=grids_.end())
                        index->grid_=found_grid->first;
                    else{
                        auto iter_grid = grids_.insert(std::make_pair(index->grid_.lock(),std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>())).first;
                        iter_grid->second.insert(index);
                        index->grid_=iter_grid->first;
                    }
                    if(auto found_common = common_.find(index->cmn_);found_common!=common_.end())
                        index->cmn_=found_common->first;
                    else{
                        auto iter_common = common_.insert(std::make_pair(data->cmn_,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>())).first;
                        iter_common->second.insert(index);
                        index->cmn_=iter_common->first;
                    }
                    if(auto found_lvl = levels_.find(data->lvl_);found_lvl!=levels_.end())
                        index->lvl_=found_lvl->first;
                    else{
                        auto iter_lvl = levels_.insert(std::make_pair(data->lvl_,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>())).first;
                        iter_lvl->second.insert(index);
                        index->lvl_=iter_lvl->first;
                    }
                    if(auto found_tf = tf_.find(data->tf_);found_tf!=tf_.end())
                        index->tf_=found_tf->first;
                    else{
                        auto iter_tf = tf_.insert(std::make_pair(data->tf_,std::unordered_set<std::weak_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal>())).first;
                        iter_tf->second.insert(index);
                        index->tf_=iter_tf->first;
                    }
                    index_.insert(index);
                }
            }
        }
    }

    template<std::ranges::range RANGE>
    void add_data(const RANGE& indexes)
        requires(std::is_same_v<typename RANGE::value_type,std::shared_ptr<IndexStruct>>)
    {
        for(auto& data:indexes){
            add_data(data);
        }
    }
    void add_data(const DataStruct& other){
        add_data(other.index_);
    }
    
    void add_data(const std::string& path,/* std::ranges::range  auto*/ const std::vector<IndexResultVariant>& grib_msg, std::error_code& err)
    // requires(std::is_same_v<typename std::decay_t<decltype(grib_msg)>::value_type,GribMsgDataInfo>)
    {   
        {
            utc_tp_t<std::chrono::seconds> last;
            bool contains_need_type = false;
            if(!std::is_sorted(grib_msg.begin(),grib_msg.end(),[&last,&contains_need_type](const IndexResultVariant& lhs,const IndexResultVariant& rhs)
                {
                    if(std::holds_alternative<GribMsgDataInfo>(lhs)){
                        last = std::get<GribMsgDataInfo>(lhs).date;
                        contains_need_type = true;
                    }
                    if(std::holds_alternative<GribMsgDataInfo>(rhs)){
                        contains_need_type = true;
                        return last<std::get<GribMsgDataInfo>(rhs).date;
                    }
                    else
                        return true;
                }))
            {
                err = std::make_error_code(std::errc::invalid_argument);
                return;
            }
            if(!contains_need_type)
                return;
        }
        auto file = std::make_shared<path::Storage<false>>(path::Storage<false>::file(path,std::chrono::system_clock::now()));
        std::unordered_set<std::shared_ptr<IndexStruct>,IndexStruct::Hash,IndexStruct::Equal> idxs_tmp;
        auto lambda = [this,&file,&idxs_tmp,&err](const auto& msg)
        {
            using T = std::decay_t<decltype(msg)>;
            if constexpr(std::is_same_v<T,GribMsgDataInfo>){
                if(msg.err_==API::ErrorData::ErrorCode<API::GRIB1>::NONE_ERR){
                    std::shared_ptr<IndexStruct> idx_tmp = std::make_shared<IndexStruct>();
                    idx_tmp->cmn_=Grib1CommonDataProperties(msg.center,msg.table_version,msg.parameter);
                    idx_tmp->lvl_=msg.level_;
                    idx_tmp->tf_=msg.t_unit;
                    if(auto found_grid =grids_.find(msg.grid_data);found_grid!=grids_.end())
                        idx_tmp->grid_=found_grid->first;
                    else idx_tmp->grid_ = std::make_shared<GridInfo>(msg.grid_data);
                    if(auto found_path = paths_.find(file);found_path!=paths_.end())
                        idx_tmp->path_=found_path->first;
                    else idx_tmp->path_ = file;
                    if(auto found = idxs_tmp.find(idx_tmp);found!=idxs_tmp.end()){
                        std::error_code err_tmp;
                        if((*found)->ts_pos_.back().first.push_time(msg.date,err) && err==std::error_code())
                            return;
                        else 
                            (*found)->ts_pos_.push_back(std::make_pair<TimeSequence,std::vector<ptrdiff_t>>(TimeSequence(msg.date),{msg.buf_pos_}));
                    }
                    else{
                        auto iter = *idxs_tmp.insert(idx_tmp).first;
                        iter->ts_pos_.push_back(std::make_pair<TimeSequence,std::vector<ptrdiff_t>>(TimeSequence(msg.date),{msg.buf_pos_}));
                    }
                }
                else{
                    err=std::make_error_code(std::errc::bad_message);
                    return;
                }
                return;
            }  
        };
        for(auto& msg:grib_msg){
            std::visit(lambda,msg);
        }
        add_data(idxs_tmp);
    }

    void add_data(const path::Storage<false>& path,const DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::find_all_t& data){
        auto file = std::make_shared<path::Storage<false>>(path);
        for(const auto& found_data:data){
            std::shared_ptr<IndexStruct> id = std::make_shared<IndexStruct>();
            id->cmn_=found_data.cmn_;
            id->grid_=found_data.add_.grid_;
            id->lvl_=found_data.add_.lvl_;
            id->path_=file;
            id->tf_=found_data.add_.fcst_;
            id->ts_pos_.push_back(std::make_pair(found_data.add_.ts_,std::vector<ptrdiff_t>()));
            add_data(id);
        }
    }
};


namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,Grib1Data::IndexStruct>{
        using type = Grib1Data::IndexStruct;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            if(msg.grid_.expired()||msg.path_.expired()||msg.path_.lock()->path_.empty())
                return SerializationEC::NONE;
            return serialize<NETWORK_ORDER>(msg,buf,msg.cmn_,msg.grid_,msg.path_,msg.lvl_,msg.tf_,msg.ts_pos_);
        }
    };

    template<>
    struct Serial_size<Grib1Data::IndexStruct>{
        using type = Grib1Data::IndexStruct;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.cmn_,msg.grid_,msg.path_,msg.lvl_,msg.tf_,msg.ts_pos_);
        }
    };

    template<>
    struct Min_serial_size<Grib1Data::IndexStruct>{
        using type = Grib1Data::IndexStruct;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::cmn_),decltype(type::grid_),decltype(type::path_),decltype(type::lvl_),decltype(type::tf_),decltype(type::ts_pos_)>();
        }();
    };

    template<>
    struct Max_serial_size<Grib1Data::IndexStruct>{
        using type = Grib1Data::IndexStruct;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::cmn_),decltype(type::grid_),decltype(type::path_),decltype(type::lvl_),decltype(type::tf_),decltype(type::ts_pos_)>();
        }();
    };

    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            if(auto err = serialize<NETWORK_ORDER>(msg.index_.size(),buf);err!=SerializationEC::NONE)
                return err;
            for(auto& index:msg.index_){
                if(index->grid_.expired()||index->path_.expired()||index->path_.lock()->path_.empty())
                    continue;
                if(auto err = serialize<NETWORK_ORDER>(index->cmn_,buf);err!=SerializationEC::NONE)
                    return err;
                if(auto err = serialize<NETWORK_ORDER>(index->grid_,buf);err!=SerializationEC::NONE)
                    return err;
                if(auto err = serialize<NETWORK_ORDER>(index->path_,buf);err!=SerializationEC::NONE)
                    return err;
                if(auto err = serialize<NETWORK_ORDER>(index->lvl_,buf);err!=SerializationEC::NONE)
                    return err;
                if(auto err = serialize<NETWORK_ORDER>(index->tf_,buf);err!=SerializationEC::NONE)
                    return err;
                if(auto err = serialize<NETWORK_ORDER>(index->ts_pos_,buf);err!=SerializationEC::NONE)
                    return err;
            }
            return serialize<NETWORK_ORDER>(msg,buf,msg.index_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            size_t sz = 0;
            msg.index_.clear();
            msg.by_diff_.clear();
            msg.by_intervals_.clear();
            msg.common_.clear();
            msg.grids_.clear();
            msg.levels_.clear();
            msg.paths_.clear();
            msg.tf_.clear();
            if(auto err = deserialize<NETWORK_ORDER>(sz,buf);err!=SerializationEC::NONE)
                return err;
            buf=buf.subspan(serial_size(sz));
            Grib1CommonDataProperties cmn;
            Level lvl;
            TimeForecast tf;
            std::shared_ptr<path::Storage<false>> path;
            std::shared_ptr<GridInfo> grid;
            std::vector<std::pair<TimeSequence,std::vector<ptrdiff_t>>> ts_positions;

            for(int i=0;i<sz;++i){
                if(auto err = deserialize<NETWORK_ORDER>(cmn,buf);err!=SerializationEC::NONE)
                    return err;
                if(auto err = deserialize<NETWORK_ORDER>(grid,buf.subspan(serial_size(cmn)));err!=SerializationEC::NONE)
                    return err;
                if(auto err = deserialize<NETWORK_ORDER>(path,buf.subspan(serial_size(grid)));err!=SerializationEC::NONE)
                    return err;
                if(auto err = deserialize<NETWORK_ORDER>(lvl,buf.subspan(serial_size(path)));err!=SerializationEC::NONE)
                    return err;
                if(auto err = deserialize<NETWORK_ORDER>(tf,buf.subspan(serial_size(lvl)));err!=SerializationEC::NONE)
                    return err;
                if(auto err = deserialize<NETWORK_ORDER>(ts_positions,buf.subspan(serial_size(tf)));err!=SerializationEC::NONE)
                    return err;
                buf=buf.subspan(serial_size(ts_positions));
                std::shared_ptr<type::IndexStruct> index = std::make_shared<type::IndexStruct>();
                index->cmn_=cmn;
                index->grid_=grid;
                index->path_=path;
                index->lvl_=lvl;
                index->tf_=tf;
                index->ts_pos_.swap(ts_positions);
                msg.add_data(index);
            }
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.index_);
        }
    };

    template<>
    struct Min_serial_size<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::index_)>();
        }();
    };

    template<>
    struct Max_serial_size<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::index_)>();
        }();
    };
}