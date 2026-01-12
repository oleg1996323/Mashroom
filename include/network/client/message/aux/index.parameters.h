#pragma once
#include <variant>
#include "network/common/message/msgdef.h"
#include "API/grib1/include/code_tables.h"
#include "API/grib1/include/sections/product/levels.h"
#include "API/grib1/include/sections/product/time_forecast.h"

namespace network{
template<Data_t T,Data_f F>
struct IndexParameters;

template<>
struct IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
    std::optional<RepresentationType> grid_type_;
    std::optional<TimeSequence> time_;
    std::optional<TimeForecast> forecast_preference_;
    std::optional<Level> level_;
    IndexParameters() = default;
    IndexParameters(IndexParameters&& other) noexcept = default;
    IndexParameters(const IndexParameters& other) noexcept = delete;
    IndexParameters& operator=(IndexParameters&& other) noexcept = default;
    IndexParameters& operator=(const IndexParameters& other) = delete;

    IndexParameters& set_grid_type(RepresentationType rep){
        grid_type_.emplace(rep);
        return *this;
    }
    IndexParameters& set_time_sequence(TimeSequence time){
        time_.emplace(time);
        return *this;
    }
    IndexParameters& set_forecast_preference(TimeForecast fcst){
        forecast_preference_.emplace(fcst);
        return *this;
    }
};
using IndexParameters_t = std::variant<std::monostate,IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.grid_type_,msg.time_,msg.forecast_preference_,msg.level_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.grid_type_,msg.time_,msg.forecast_preference_,msg.level_);
        }
    };

    template<>
    struct Serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.grid_type_,msg.time_,msg.forecast_preference_,msg.level_);
        }
    };

    template<>
    struct Min_serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::grid_type_),decltype(type::time_),
            decltype(type::forecast_preference_),decltype(type::level_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::grid_type_),decltype(type::time_),
            decltype(type::forecast_preference_),decltype(type::level_)>();
        }();
    };
}