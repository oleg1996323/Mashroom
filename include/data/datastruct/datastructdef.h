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
#include "types/time_interval.h"
#include "types/coord.h"
#include "definitions/def.h"
#include "definitions/path_process.h"
#include "API/grib1/include/sections/grid/grid.h"
template<Data_t TYPE,Data_f FORMAT>
struct DataStruct;

struct AbstractDataStruct{
    constexpr virtual Data_f format_type() const noexcept = 0;
    constexpr virtual Data_t data_type() const noexcept = 0;
    virtual ~AbstractDataStruct() = default;
};

namespace find_data_info::details{
    template<Data_t ADD_T,Data_f ADD_F>
    struct Additional;

    template<>
    struct Additional<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
        std::shared_ptr<GridInfo> grid_;
        TimeSequence ts_;
        Level lvl_;
        TimeForecast fcst_;
    };
}

template<Data_t TYPE,Data_f FORMAT>
struct FoundSublimedDataInfo{
    find_data_info::details::Additional<TYPE,FORMAT> add_;
    CommonDataProperties<TYPE,FORMAT> cmn_;
};

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

        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
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
    struct Serialize<NETWORK_ORDER,FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.cmn_,msg.add_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>;

        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.cmn_,msg.add_);
        }
    };

    template<>
    struct Serial_size<FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.cmn_,msg.add_);
        }
    };

    template<>
    struct Min_serial_size<FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::cmn_),decltype(type::add_)>();
        }();
    };

    template<>
    struct Max_serial_size<FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::cmn_),decltype(type::add_)>();
        }();
    };
}

template<>
struct std::hash<std::unique_ptr<AbstractDataStruct>>{
    using is_transparent = std::true_type;

    size_t operator()(const AbstractDataStruct& val) const{
        return static_cast<int>(val.format_type())<<sizeof(size_t)/2+static_cast<int>(val.data_type());
    }
    size_t operator()(const std::unique_ptr<AbstractDataStruct>& val) const{
        return val?static_cast<int>(val->format_type())<<sizeof(size_t)/2+static_cast<int>(val->data_type()):0;
    }
    size_t operator()(std::pair<Data_t, Data_f> tags) const{
        return static_cast<int>(tags.second)<<sizeof(size_t)/2+static_cast<int>(tags.first);
    }
    size_t operator()(std::pair<Data_f, Data_t> tags) const{
        return static_cast<int>(tags.first)<<sizeof(size_t)/2+static_cast<int>(tags.second);
    }
    size_t operator()(Data_t type, Data_f format) const{
        return static_cast<int>(format)<<sizeof(size_t)/2+static_cast<int>(type);
    }
    size_t operator()(Data_f format,Data_t type) const{
        return static_cast<int>(format)<<sizeof(size_t)/2+static_cast<int>(type);
    }
};

template<>
struct std::equal_to<std::unique_ptr<AbstractDataStruct>>{
    using is_transparent = std::true_type;

    bool operator()(const AbstractDataStruct& lhs,const AbstractDataStruct& rhs) const{
        return lhs.format_type()==rhs.format_type() && lhs.data_type()==rhs.data_type();
    }
    bool operator()(const std::unique_ptr<AbstractDataStruct>& lhs,const std::unique_ptr<AbstractDataStruct>& rhs) const{
        return (lhs && rhs)?(lhs->format_type()==rhs->format_type() && lhs->data_type()==rhs->data_type()):(!lhs && !rhs);
    }
    bool operator()(Data_t type, Data_f format, const AbstractDataStruct& val) const{
        return val.format_type()==format && val.data_type()==type;
    }
    bool operator()(const AbstractDataStruct& val,Data_t type, Data_f format) const{
        return this->operator()(type,format,val);
    }
    bool operator()(const std::pair<Data_t,Data_f>& tags, const std::unique_ptr<AbstractDataStruct>& val) const{
        return val && val->format_type()==tags.second && val->data_type()==tags.first;
    }
    bool operator()(const std::pair<Data_f,Data_t>& tags, const std::unique_ptr<AbstractDataStruct>& val) const{
        return val && val->format_type()==tags.first && val->data_type()==tags.second;
    }
    bool operator()(const std::unique_ptr<AbstractDataStruct>& val,const std::pair<Data_t,Data_f>& tags) const{
        return this->operator()(tags,val);
    }
    bool operator()(const std::unique_ptr<AbstractDataStruct>& val,const std::pair<Data_f,Data_t>& tags) const{
        return this->operator()(tags,val);
    }
};