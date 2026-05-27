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

using Grib1Data = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

template<>
struct DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
    struct IndexStructDeserialize{
        std::shared_ptr<GridInfo> grid_;
        std::shared_ptr<path::Storage<false>> path_;
        std::vector<std::pair<TimeSequence,std::deque<ptrdiff_t>>> ts_pos_;
        TimeForecast tf_;
        Grib1CommonDataProperties cmn_;
        Level lvl_;
    };

    struct IndexStruct{
        std::weak_ptr<GridInfo> grid_;
        std::weak_ptr<path::Storage<false>> path_;
        std::vector<std::pair<TimeSequence,std::deque<ptrdiff_t>>> ts_pos_;
        TimeForecast tf_;
        Grib1CommonDataProperties cmn_;
        Level lvl_;
        IndexStruct(IndexStructDeserialize&& other):
            grid_(other.grid_),
            path_(other.path_),
            ts_pos_(std::move(other.ts_pos_)),
            tf_(std::move(other.tf_)),
            cmn_(std::move(other.cmn_)),
            lvl_(std::move(other.lvl_))
        {}
        constexpr Data_t type() const noexcept{
            return Data_t::TIME_SERIES;
        }
        constexpr Data_f format() const noexcept{
            return Data_f::GRIB_v1;
        }

        bool operator==(const IndexStruct& other) const;
        bool operator!=(const IndexStruct& other) const;
        struct Hash{
            using is_transparent = std::true_type;
            size_t operator()(const IndexStruct& val) const;
            size_t operator()(const std::weak_ptr<IndexStruct>& val) const;
            size_t operator()(const std::shared_ptr<IndexStruct>& val) const;
        };

        struct Equal{
            using is_transparent = std::true_type;
            bool operator()(const IndexStruct& lhs,
                    const IndexStruct& rhs) const;
            bool operator()(const std::shared_ptr<IndexStruct>& lhs,
                    const IndexStruct& rhs) const;
            bool operator()(const IndexStruct& lhs,
                    const std::shared_ptr<IndexStruct>& rhs) const;
            bool operator()(const std::weak_ptr<IndexStruct>& lhs,
                    const IndexStruct& rhs) const;
            bool operator()(const IndexStruct& lhs,
                    const std::weak_ptr<IndexStruct>& rhs) const;
            bool operator()(const std::shared_ptr<IndexStruct>& lhs,
                    const std::shared_ptr<IndexStruct>& rhs) const;
            bool operator()(const std::weak_ptr<IndexStruct>& lhs,
                    const std::shared_ptr<IndexStruct>& rhs) const;
            bool operator()(const std::shared_ptr<IndexStruct>& lhs,
                    const std::weak_ptr<IndexStruct>& rhs) const;
            bool operator()(const std::weak_ptr<IndexStruct>& lhs,
                    const std::weak_ptr<IndexStruct>& rhs) const;
        };

        IndexStruct() = default;
        IndexStruct(const IndexStruct& other):
            grid_(other.grid_),
            path_(other.path_),
            ts_pos_(other.ts_pos_),
            tf_(other.tf_),
            cmn_(other.cmn_),
            lvl_(other.lvl_){}
        IndexStruct(IndexStruct&& other):
            grid_(std::move(other.grid_)),
            path_(std::move(other.path_)),
            ts_pos_(std::move(other.ts_pos_)),
            tf_(other.tf_),cmn_(other.cmn_),
            lvl_(other.lvl_){}
    };
    
    std::unordered_map<std::shared_ptr<path::Storage<false>>,
        std::unordered_set<std::weak_ptr<IndexStruct>,
        IndexStruct::Hash,IndexStruct::Equal>> paths_;
    std::unordered_map<std::shared_ptr<GridInfo>,
        std::unordered_set<std::weak_ptr<IndexStruct>,
        IndexStruct::Hash,IndexStruct::Equal>> grids_;
    std::unordered_map<Level,std::unordered_set<std::weak_ptr<IndexStruct>,
        IndexStruct::Hash,IndexStruct::Equal>> levels_;
    std::unordered_map<TimeForecast,std::unordered_set<
        std::weak_ptr<IndexStruct>,
        IndexStruct::Hash,IndexStruct::Equal>> tf_;
    std::unordered_map<Grib1CommonDataProperties,std::unordered_set<
        std::weak_ptr<IndexStruct>,
        IndexStruct::Hash,IndexStruct::Equal>> common_;
    std::map<TimeInterval,std::unordered_set<std::weak_ptr<IndexStruct>,
        IndexStruct::Hash,IndexStruct::Equal>> by_intervals_;
    std::map<DateTimeDiff,std::unordered_set<std::weak_ptr<IndexStruct>,
        IndexStruct::Hash,IndexStruct::Equal>> by_diff_;
    std::unordered_set<std::shared_ptr<IndexStruct>,IndexStruct::Hash,
        IndexStruct::Equal> index_;

    DataStruct() = default;
    DataStruct(const DataStruct&) = delete;
    DataStruct(DataStruct&& other):
    paths_(std::move(other.paths_)),
    grids_(std::move(other.grids_)),
    levels_(std::move(other.levels_)),
    tf_(std::move(other.tf_)),
    common_(std::move(other.common_)),
    by_intervals_(std::move(other.by_intervals_)),
    by_diff_(std::move(other.by_diff_)),
    index_(std::move(other.index_))
    {}

    std::vector<std::pair<path::Storage<false>,std::vector<ptrdiff_t>>> match_files(
        utc_tp last_update,
        Coord pos,
        Organization center,
        const std::unordered_set<SearchParamTableVersion>& param_tables,
        std::optional<utc_tp_t<std::chrono::seconds>> from,
        std::optional<utc_tp_t<std::chrono::seconds>> to,
        std::optional<DateTimeDiff> diff,
        std::optional<TimeForecast> forecast_preferences,
        std::optional<Level> level_,
        std::optional<RepresentationType> rep_t
        ) const;

    std::vector<ptrdiff_t> match(
        std::string_view path,
        utc_tp last_update,
        Coord,
        Organization center,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        std::optional<utc_tp_t<std::chrono::seconds>>,
        std::optional<utc_tp_t<std::chrono::seconds>>,
        std::optional<DateTimeDiff> diff,
        std::optional<TimeForecast> time_fcst,
        std::optional<Level> level_,
        std::optional<RepresentationType> rep_t
    ) const;

    std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>find_all(
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
        std::optional<RepresentationType> grid_type) const;

    using match_files_t = std::invoke_result_t<decltype(&DataStruct::match_files),
        DataStruct*,
        utc_tp,
        Coord,
        Organization,
        const std::unordered_set<SearchParamTableVersion>&,
        std::optional<utc_tp_t<std::chrono::seconds>>,
        std::optional<utc_tp_t<std::chrono::seconds>>,
        std::optional<DateTimeDiff>,
        std::optional<TimeForecast>,
        std::optional<Level>,
        std::optional<RepresentationType>>;

    using match_t = std::invoke_result_t<decltype(&DataStruct::match),
        DataStruct*,
        std::string_view,
        utc_tp,
        Coord,
        Organization,
        const std::unordered_set<SearchParamTableVersion>&,
        std::optional<utc_tp_t<std::chrono::seconds>>,
        std::optional<utc_tp_t<std::chrono::seconds>>,
        std::optional<DateTimeDiff>,
        std::optional<TimeForecast>,
        std::optional<Level>,
        std::optional<RepresentationType>>;
    using find_all_t = std::invoke_result_t<decltype(&DataStruct::find_all),
        DataStruct*,
        const std::unordered_set<
        CommonDataProperties<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>>&,
        utc_tp,
        std::optional<Lat>,
        std::optional<Lat>,
        std::optional<Lon>,
        std::optional<Lon>,
        std::optional<utc_tp_t<std::chrono::seconds>>,
        std::optional<utc_tp_t<std::chrono::seconds>>,
        std::optional<DateTimeDiff>,
        std::optional<std::pair<TimeForecast,TimeForecast::COMPARISION_TYPE>>,
        std::optional<std::pair<Level,Level::COMPARISION_TYPE>>,
        std::optional<RepresentationType>>;

    void delete_index(const path::Storage<false>& path);

    void rewrite_index(const std::shared_ptr<IndexStruct>& data);

    template<std::ranges::range RANGE>
    void rewrite_indexes(const RANGE& indexes)
        requires(std::is_same_v<typename RANGE::value_type,
                std::shared_ptr<IndexStruct>>)
    {
        for(auto& data:indexes){
            rewrite_index(data);
        }
    }
    void update_indexing(const DataStruct& other){
        rewrite_indexes(other.index_);
    }
    void add_data(const path::Storage<false>& path,
        const std::vector<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>& grib_msg,
        std::error_code& err);

    void add_data(const path::Storage<false>& path,
        const DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::find_all_t& data);

    bool operator==(const DataStruct& other) const;
};

namespace operators{

bool operator==(const DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct& lhs,
        const DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct& rhs);
template<template<typename TYPE> typename CLASS>
requires(smart_pointer_concept<CLASS<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>> || 
        weak_pointer_concept<CLASS<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>>)
bool operator==(const CLASS<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>& lhs,
        const DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct& rhs){
    return DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct::Equal()(lhs,rhs);
}
template<template<typename TYPE> typename CLASS>
requires(smart_pointer_concept<CLASS<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>> || 
        weak_pointer_concept<CLASS<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>>)
bool operator==(const DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct& lhs,
        const CLASS<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>& rhs){
    return DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct::Equal()(lhs,rhs);
}
template<template<typename TYPE> typename CLASS1,
        template<typename TYPE> typename CLASS2>
requires((smart_pointer_concept<CLASS1<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>> || 
        weak_pointer_concept<CLASS1<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>>) &&
        (smart_pointer_concept<CLASS2<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>> || 
        weak_pointer_concept<CLASS2<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>>))
bool operator==(const CLASS1<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>& lhs,
        const CLASS2<DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct>& rhs){
    return DataStruct<Data_t::TIME_SERIES,
        Data_f::GRIB_v1>::IndexStruct::Equal()(lhs,rhs);
}
}

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,Grib1Data::IndexStruct>{
        using type = Grib1Data::IndexStruct;
        SerializationEC operator()(const type& msg,
            std::vector<char>& buf) const noexcept{
                return serialize<NETWORK_ORDER>(msg,buf,msg.cmn_,
                    msg.grid_,msg.path_,msg.lvl_,msg.tf_,msg.ts_pos_);
        }
    };

    template<>
    struct Serial_size<Grib1Data::IndexStruct>{
        using type = Grib1Data::IndexStruct;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.cmn_,msg.grid_,
                    msg.path_,msg.lvl_,
                    msg.tf_,msg.ts_pos_);
        }
    };

    template<>
    struct Min_serial_size<Grib1Data::IndexStruct>{
        using type = Grib1Data::IndexStruct;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::cmn_),
                decltype(type::grid_),decltype(type::path_),
                decltype(type::lvl_),decltype(type::tf_),
                decltype(type::ts_pos_)>();
        }();
    };

    template<>
    struct Max_serial_size<Grib1Data::IndexStruct>{
        using type = Grib1Data::IndexStruct;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::cmn_),
                decltype(type::grid_),decltype(type::path_),
                decltype(type::lvl_),decltype(type::tf_),
                decltype(type::ts_pos_)>();
        }();
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,Grib1Data::IndexStructDeserialize>{
        using type = Grib1Data::IndexStructDeserialize;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.cmn_,msg.grid_,
                    msg.path_,msg.lvl_,
                    msg.tf_,msg.ts_pos_);
        }
    };

    template<>
    struct Serial_size<Grib1Data::IndexStructDeserialize>{
        using type = Grib1Data::IndexStructDeserialize;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.cmn_,msg.grid_,
                    msg.path_,msg.lvl_,
                    msg.tf_,msg.ts_pos_);
        }
    };

    template<>
    struct Min_serial_size<Grib1Data::IndexStructDeserialize>{
        using type = Grib1Data::IndexStructDeserialize;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::cmn_),decltype(type::grid_),
                    decltype(type::path_),decltype(type::lvl_),
                    decltype(type::tf_),decltype(type::ts_pos_)>();
        }();
    };

    template<>
    struct Max_serial_size<Grib1Data::IndexStructDeserialize>{
        using type = Grib1Data::IndexStructDeserialize;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::cmn_),decltype(type::grid_),
                    decltype(type::path_),decltype(type::lvl_),
                    decltype(type::tf_),decltype(type::ts_pos_)>();
        }();
    };

    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,
        DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg,
            std::vector<char>& buf) const noexcept{
                return serialize<NETWORK_ORDER>(msg,buf,msg.index_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,
        DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(type& msg,
            StreamSerializer& buf) const noexcept{
                size_t sz = 0;
                if(buf.advance_if_deserialized(sz)){

                }
                else{
                    if(auto err = deserialize<NETWORK_ORDER>(sz,buf);
                        err!=SerializationEC::NONE)
                        return err;
                    msg.index_.clear();
                    msg.by_diff_.clear();
                    msg.by_intervals_.clear();
                    msg.common_.clear();
                    msg.grids_.clear();
                    msg.levels_.clear();
                    msg.paths_.clear();
                    msg.tf_.clear();
                    buf.set_container_size(sz);
                }
                sz = buf.remained_container_elements();
                for(int i=0;i<sz;++i){
                    std::shared_ptr<type::IndexStructDeserialize> tmp;
                    if(auto err = deserialize<NETWORK_ORDER>(tmp,buf);
                        err!=SerializationEC::NONE)
                            return err;
                    else buf.commit_container_elem();
                    if(!tmp)
                        continue;
                    else{
                        auto tmp_grid = tmp->grid_;
                        auto tmp_path = tmp->path_;
                        std::shared_ptr<type::IndexStruct> index=
                            std::make_shared<type::IndexStruct>(std::move(*tmp));
                        msg.rewrite_index(index);
                    }
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