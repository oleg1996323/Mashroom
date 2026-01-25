#pragma once
#include <variant>
#include "network/common/message/msgdef.h"
#include "API/grib1/include/code_tables.h"
#include "API/grib1/include/sections/product/levels.h"
#include "API/grib1/include/sections/product/time_forecast.h"
#include "types/rect.h"
#include "data/common_data_properties.h"
#include <unordered_set>
#include <optional>
#include "definitions/def.h"

namespace network{
template<Data_t T,Data_f F>
struct IndexParameters;

template<>
struct IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
    std::unordered_set<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>> common_;
    std::optional<TimeInterval> tinterval_;
    std::optional<DateTimeDiff> tdiff_;
    std::optional<TimeForecast> forecast_preference_;
    std::optional<Level> level_;
    std::optional<Lat> top_;
    std::optional<Lat> bottom_;
    std::optional<Lon> left_;
    std::optional<Lon> right_;
    std::optional<RepresentationType> grid_type_;
    IndexParameters() = default;
    IndexParameters(IndexParameters&& other) noexcept = default;
    IndexParameters(const IndexParameters& other) noexcept = delete;
    IndexParameters& operator=(IndexParameters&& other) noexcept = default;
    IndexParameters& operator=(const IndexParameters& other) = delete;

    IndexParameters& set_grid_type(RepresentationType rep){
        grid_type_.emplace(rep);
        return *this;
    }
    IndexParameters& set_time_interval(TimeInterval time){
        tinterval_.emplace(time);
        return *this;
    }
    IndexParameters& set_time_diff(DateTimeDiff diff){
        tdiff_.emplace(diff);
        return *this;
    }
    IndexParameters& set_position_rect(std::optional<Lat> top,std::optional<Lat> bottom,std::optional<Lon> left, std::optional<Lon> right) noexcept{
        top_=top;
        bottom_=bottom;
        left_=left;
        right_=right;
        return *this;
    }
    IndexParameters& set_forecast_preference(TimeForecast fcst){
        forecast_preference_.emplace(fcst);
        return *this;
    }
    IndexParameters& set_common_data_properties(
        const std::unordered_set<
            CommonDataProperties<Data_t::TIME_SERIES,
                                Data_f::GRIB_v1>>& cmn)
    {
        common_=cmn;
        return *this;
    }
    IndexParameters& add_common_data_property(
        CommonDataProperties<Data_t::TIME_SERIES,
                                Data_f::GRIB_v1> cmn)
    {
        common_.insert(cmn);
        return *this;
    }
};
using IndexParameters_t = std::variant<std::monostate,
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.common_,msg.tinterval_,
                msg.tdiff_,msg.forecast_preference_,msg.level_,
                msg.top_,msg.bottom_,msg.left_,msg.right_,msg.grid_type_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.common_,msg.tinterval_,
                msg.tdiff_,msg.forecast_preference_,msg.level_,
                msg.top_,msg.bottom_,msg.left_,msg.right_,msg.grid_type_);
        }
    };

    template<>
    struct Serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.common_,msg.tinterval_,msg.tdiff_,
                msg.forecast_preference_,msg.level_,msg.top_,msg.bottom_,msg.left_,msg.right_,msg.grid_type_);
        }
    };

    template<>
    struct Min_serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::common_),
            decltype(type::tinterval_),decltype(type::tdiff_),
            decltype(type::forecast_preference_),decltype(type::level_),
            decltype(type::top_),decltype(type::bottom_),
            decltype(type::left_),decltype(type::right_),
            decltype(type::grid_type_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::common_),
            decltype(type::tinterval_),decltype(type::tdiff_),
            decltype(type::forecast_preference_),decltype(type::level_),
            decltype(type::top_),decltype(type::bottom_),
            decltype(type::left_),decltype(type::right_),
            decltype(type::grid_type_)>();
        }();
    };
}