#pragma once
#include "data/def.h"
#include "data/common_data_properties.h"
#include <algorithm>
#include <numeric>
#include <ranges>
#include <unordered_set>
#include <vector>
#include <map>
#include <optional>
#include <cstdint>
#include "OsterLib/types/time_interval.h"
#include "OsterLib/types/coord.h"
#include "definitions/def.h"
#include "Location.h"
#include "grib1/sections.h"
template<Data_t TYPE,Data_f FORMAT>
struct DataStruct;

namespace find_data_info::details{
    template<Data_t ADD_T,Data_f ADD_F>
    struct Additional;

    template<>
    struct Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
        std::shared_ptr<GridInfo> grid_;
        TimeSequence ts_;
        Level lvl_;
        TimeForecast fcst_;
        bool operator==(const Additional& other) const{
            if((!grid_ && other.grid_) ||
                (grid_ && !other.grid_))
                return false;
            return (grid_?*grid_==*other.grid_:false)&&
                ts_==other.ts_ && lvl_==other.lvl_ &&
                fcst_ == other.fcst_;
        }
        bool operator!=(const Additional& other) const{
            return !(*this==other);
        }
    };
}

template<Data_t TYPE,Data_f FORMAT>
struct SearchDataResult{
    find_data_info::details::Additional<TYPE,FORMAT> add_ = {};
    CommonDataProperties<TYPE,FORMAT> cmn_ = {};
    using Additional_t = decltype(add_);
    bool operator==(const SearchDataResult& other) const{
        return cmn_==other.cmn_ && add_==other.add_;
    }
    bool operator!=(const SearchDataResult& other) const{
        return !(*this==other);
    }
};

template<>
std::expected<find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> 
    from_json<find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& val);

template<>
boost::json::value to_json(const 
        find_data_info::details::Additional<
            Data_t::TIME_SERIES,Data_f::GRIB_v1>& val);

template<Data_t TYPE,Data_f FORMAT>
std::expected<SearchDataResult<TYPE,FORMAT>,std::exception> 
    from_json(const boost::json::value& val)
{
    if(val.is_object()){
        auto& obj = val.as_object();
        if(!obj.contains("common"))
            return std::unexpected(std::invalid_argument(
            "missing \"common\" field at parsing json data of SearchDataResult type"));
        if(!obj.contains("additional"))
            return std::unexpected(std::invalid_argument(
            "missing \"additional\" field at parsing json data of SearchDataResult type"));
        SearchDataResult<TYPE,FORMAT> result;
        if(auto cmn_parse = from_json<decltype(result.cmn_)>(obj.at("common"));
                !cmn_parse.has_value())
            return std::unexpected(cmn_parse.error());
        else result.cmn_ = std::move(cmn_parse.value());
        if(auto add_parse = from_json<decltype(result.add_)>(obj.at("additional"));
                !add_parse.has_value())
            return std::unexpected(add_parse.error());
        else result.add_ = std::move(add_parse.value());
        return result;
    }
    else if(val.is_null())
        return SearchDataResult<TYPE,FORMAT>();
    else return std::unexpected(std::invalid_argument(
            "not object-type SearchDataResult at parsing json data"));
}

template<Data_t TYPE,Data_f FORMAT>
boost::json::value to_json(const SearchDataResult<TYPE,FORMAT>& val)
{
    boost::json::object result;
    result["common"]=to_json(val.cmn_);
    result["additional"]=to_json(val.add_);
    return result;
}

namespace serialization{

    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.ts_,msg.fcst_,msg.grid_,msg.lvl_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.ts_,msg.fcst_,msg.grid_,msg.lvl_);
        }
    };

    template<>
    struct Serial_size<find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.ts_,msg.fcst_,msg.grid_,msg.lvl_);
        }
    };

    template<>
    struct Min_serial_size<find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::ts_),decltype(type::fcst_),decltype(type::grid_),decltype(type::lvl_)>();
        }();
    };

    template<>
    struct Max_serial_size<find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = find_data_info::details::Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::ts_),decltype(type::fcst_),decltype(type::grid_),decltype(type::lvl_)>();
        }();
    };

    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.cmn_,msg.add_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.cmn_,msg.add_);
        }
    };

    template<>
    struct Serial_size<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.cmn_,msg.add_);
        }
    };

    template<>
    struct Min_serial_size<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::cmn_),decltype(type::add_)>();
        }();
    };

    template<>
    struct Max_serial_size<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::cmn_),decltype(type::add_)>();
        }();
    };
}