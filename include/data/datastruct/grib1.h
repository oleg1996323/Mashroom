#pragma once
#include "datastructdef.h"
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

using Grib1Data = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

template<>
struct DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>:public AbstractDataStruct{
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
        std::shared_ptr<GridInfo> grid_;
        path::Storage<false> path_;
        std::vector<ptrdiff_t> pos_;
        TimeForecast tf_;
        TimeSequence ts_;
        Grib1CommonDataProperties cmn_;
        Level lvl_;

        struct Hash{
            size_t operator()(const IndexStruct& val) const{
                size_t hash = 0;
            using namespace boost;
                hash_combine(hash,std::hash<std::shared_ptr<GridInfo>>()(val.grid_));
                hash_combine(hash,std::hash<path::Storage<false>>()(val.path_));
                hash_combine(hash,std::hash<TimeForecast>()(val.tf_));
                hash_combine(hash,std::hash<TimeSequence>()(val.ts_));
                hash_combine(hash,std::hash<Grib1CommonDataProperties>()(val.cmn_));
                hash_combine(hash,std::hash<Level>()(val.lvl_));
                return hash;
            }
        };

        IndexStruct() = default;
        IndexStruct(const IndexStruct& other):
        grid_(other.grid_),path_(other.path_),pos_(other.pos_),tf_(other.tf_),ts_(other.ts_),cmn_(other.cmn_),lvl_(other.lvl_){}
        IndexStruct(IndexStruct&& other):
        grid_(std::move(other.grid_)),path_(std::move(other.path_)),pos_(std::move(other.pos_)),tf_(other.tf_),ts_(other.ts_),cmn_(other.cmn_),lvl_(other.lvl_){}
    };
    // std::unordered_map<path::Storage<false>,std::vector<ptrdiff_t>> positions_;
    // sublimed_data_by_common_data by_common_data_;
    // sublimed_data_by_date_time by_date_;
    // sublimed_data_by_grid by_grid_;
    // sublimed_data_by_forecast by_forecast_;
    // sublimed_data_by_level by_level_;

    std::unordered_set<IndexStruct> index_;

    DataStruct() = default;
    DataStruct(const DataStruct&) = delete;
    DataStruct(DataStruct&& other):
    index_(std::move(other.index_)){}

    constexpr virtual Data_f format_type() const noexcept override{
        return Data_f::GRIB_v1;
    }
    constexpr virtual Data_t data_type() const noexcept override{
        return Data_t::TIME_SERIES;
    }

    void add_data(const DataStruct& other){
        for(auto& data:other.index_){
            if(auto found = index_.find(data);found!=index_.end()){
                IndexStruct idx_tmp;
                idx_tmp.cmn_=found->cmn_;
                idx_tmp.grid_=found->grid_;
                idx_tmp.lvl_=found->lvl_;
                idx_tmp.path_=found->path_;
                idx_tmp.tf_=found->tf_;
                idx_tmp.ts_=found->ts_;
                std::set_union(found->pos_.begin(),found->pos_.end(),data.pos_.begin(),data.pos_.end(),std::back_inserter(idx_tmp.pos_));
                index_.insert(std::move(idx_tmp));
            }
            else
                index_.insert(data);
        }
    }
    
    void add_data(const std::string& path,/* std::ranges::range  auto*/ const std::vector<GribMsgDataInfo>& grib_msg)
    // requires(std::is_same_v<typename std::decay_t<decltype(grib_msg)>::value_type,GribMsgDataInfo>)
    {
        for(auto& msg:grib_msg){
            if(msg.err_==API::ErrorData::ErrorCode<API::GRIB1>::NONE_ERR){
                IndexStruct idx_tmp;
                idx_tmp.cmn_=Grib1CommonDataProperties(msg.center,msg.table_version,msg.parameter);
                idx_tmp.grid_=std::make_shared<GridInfo>(msg.grid_data);
                idx_tmp.lvl_=msg.level_;
                idx_tmp.path_=path::Storage<false>::file(path,std::chrono::system_clock::now());;
                idx_tmp.tf_=msg.t_unit;
                idx_tmp.ts_=msg.date;
            }
            else continue;
        }
        // std::ranges::sort(times_pos,std::less(),&std::decay_t<decltype(times_pos)>::value_type::second);
        // std::vector<utc_tp> times;
        // std::vector<ptrdiff_t> pos;
        // {
        //     auto el_times = std::views::elements<0>(times_pos);
        //     times = std::ranges::to<std::vector<utc_tp>>(el_times);
        //     auto el_pos = std::views::elements<1>(times_pos);
        //     pos = std::ranges::to<std::vector<ptrdiff_t>>(el_pos);
        // }
        // std::error_code err;
        // auto first = times.cbegin();
        // while(err==std::error_code()){
        //     auto ts_iter = TimeSequence::make_from_range(std::span<utc_tp>(times).subspan(first-times.begin(),times.end()-first),err);
        //     if(err!=std::error_code())
        //         return;
        //     else {
        //         by_date_[ts_iter.first][file].assign_range(std::span<utc_tp>(times).subspan(first-times.begin(),ts_iter.second-first));
        //         if(ts_iter.second!=times.end())
        //             first = ts_iter.second;
        //         else return;
        //     }
        // }
    }

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

    std::vector<FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>> find_all(std::optional<RepresentationType> grid_type_,
                        std::optional<TimeSequence> time_,
                        std::optional<TimeForecast> forecast_preference_,
                        std::optional<Level> level_,
                        std::optional<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>> common_,
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
                                                    std::optional<TimeSequence>,
                                                    std::optional<TimeForecast>,
                                                    std::optional<Level>,
                                                    std::optional<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>,
                                                    utc_tp>;
};


namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.positions_,msg.by_common_data_,msg.by_date_,msg.by_grid_,msg.by_forecast_,msg.by_level_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

        template<template<typename KEY,typename MAP> typename CONTAINER,typename KEY,typename MAP>
        SerializationEC deserialize_and_check_paths(CONTAINER<KEY,MAP>& to_deserialize,const decltype(type::positions_)& positions,std::span<const char> buf){
            size_t sz = 0;
            if(auto err = deserialize<NETWORK_ORDER>(sz,buf);err!=SerializationEC::NONE)
                return err;
            std::pair<KEY,std::unordered_map<path::Storage<false>,std::vector<ptrdiff_t>>> pair_tmp;
            for(size_t i=0;i<sz;++i){
                if(auto err = deserialize<NETWORK_ORDER>(to_deserialize,buf);err!=SerializationEC::NONE)
                    return err;
                else{
                    if constexpr (smart_pointer_concept<KEY>)
                        if(pair_tmp.first.get()==nullptr)
                            continue;
                    if(!pair_tmp.second.empty()){
                        auto& values = to_deserialize[pair_tmp.first];
                        for(auto& [path,ptrs]:pair_tmp.second){
                            if(auto found = positions.find(path);found==positions.end())
                                return SerializationEC::FILE_READING_ERROR;
                            else values[found->first]=std::move(ptrs);
                        }
                    }
                }
            }
        }

        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            std::decay_t<decltype(msg.positions_)> positions_tmp_;
            if(auto err = deserialize<NETWORK_ORDER>(positions_tmp_,buf);err!=SerializationEC::NONE)
                return err;
            std::unordered_map<std::shared_ptr<Grib1CommonDataProperties>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>> cmn_tmp;
            buf = buf.subspan(serial_size(positions_tmp_));
            deserialize_and_check_paths(cmn_tmp,positions_tmp_,buf);
            std::map<TimeSequence,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>> ts_tmp;
            buf = buf.subspan(serial_size(cmn_tmp));
            deserialize_and_check_paths(ts_tmp,positions_tmp_,buf);
            std::unordered_map<std::shared_ptr<GridInfo>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>> grid_tmp;
            buf = buf.subspan(serial_size(ts_tmp));
            deserialize_and_check_paths(grid_tmp,positions_tmp_,buf);
            std::unordered_map<std::shared_ptr<TimeForecast>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>> fcst_tmp;
            buf = buf.subspan(serial_size(grid_tmp));
            deserialize_and_check_paths(fcst_tmp,positions_tmp_,buf);
            std::unordered_map<std::shared_ptr<Level>,std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>>> lvl_tmp;
            buf = buf.subspan(serial_size(fcst_tmp));
            deserialize_and_check_paths(lvl_tmp,positions_tmp_,buf);

            msg.positions_ = std::move(positions_tmp_);
            msg.by_common_data_ = std::move(cmn_tmp);
            msg.by_date_ = std::move(ts_tmp);
            msg.by_grid_ = std::move(grid_tmp);
            msg.by_forecast_ = std::move(fcst_tmp);
            msg.by_level_ = std::move(lvl_tmp);
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.positions_,msg.by_common_data_,msg.by_date_,msg.by_grid_,msg.by_forecast_,msg.by_level_);
        }
    };

    template<>
    struct Min_serial_size<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::positions_),decltype(type::by_common_data_),
                                    decltype(type::by_date_),decltype(type::by_grid_),
                                    decltype(type::by_forecast_),decltype(type::by_level_)>();
        }();
    };

    template<>
    struct Max_serial_size<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::positions_),decltype(type::by_common_data_),
                                    decltype(type::by_date_),decltype(type::by_grid_),
                                    decltype(type::by_forecast_),decltype(type::by_level_)>();
        }();
    };
}