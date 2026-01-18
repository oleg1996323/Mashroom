#pragma once
#include "data/def.h"
#include "byte_order.h"
#include "types/time_interval.h"
#include <variant>
#include "serialization.h"

template<Data_t T,Data_f F>
struct ExtractedValue;

namespace procedures::extract::details{
    template<Data_t,Data_f>
    struct ExtractedValueAdditional:std::monostate{};

    template<>
    struct ExtractedValueAdditional<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
        uint16_t T = 0;
    };
}

template<Data_f FORMAT>
struct ExtractedValue<Data_t::TIME_SERIES,FORMAT>
{
    using value_t = float;
    using time_type = utc_tp_t<nanoseconds>;
    using fcst_type = uint32_t;
    time_type time_date;
    value_t value = UNDEFINED;

    ExtractedValue() = default;
    ExtractedValue(utc_tp time, value_t val) : time_date(time), value(val){}
    ExtractedValue(const ExtractedValue &other)
    {
        if (this != &other)
        {
            time_date = other.time_date;
            value = other.value;
        }
    }
    ExtractedValue(ExtractedValue &&other) noexcept
    {
        if (this != &other)
        {
            time_date = other.time_date;
            value = other.value;

        }
    }

    bool operator<(const ExtractedValue &other) const
    {
        return time_date < other.time_date;
    }

    bool operator>(const ExtractedValue &other) const
    {
        return time_date > other.time_date;
    }

    bool operator==(const ExtractedValue& other) const noexcept{
        return value==other.value && time_date == other.time_date;
    }

    ExtractedValue &operator=(const ExtractedValue &extracted_val)
    {
        if (this != &extracted_val)
        {
            time_date = extracted_val.time_date;
            value = extracted_val.value;
        }
        return *this;
    }
    ExtractedValue &operator=(ExtractedValue &&extracted_val) noexcept
    {
        if (this != &extracted_val)
        {
            time_date = extracted_val.time_date;
            value = extracted_val.value;
        }
        return *this;
    }
};

template <Data_t T,Data_f F>
struct std::less<ExtractedValue<T,F>>
{
    bool operator()(const ExtractedValue<T,F> &lhs, const ExtractedValue<T,F> &rhs) const
    {
        return lhs < rhs;
    }
    bool operator()(const ExtractedValue<T,F> &lhs, const ExtractedValue<T,F> &rhs)
    {
        return lhs < rhs;
    }
};

template <>
class std::hash<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>
{
    size_t operator()(const ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1> &data)
    {
        return std::hash<int64_t>{}(static_cast<int64_t>(duration_cast<hours>(data.time_date.time_since_epoch()).count()) << 32 | static_cast<int64_t>(data.value));
    }
};




namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        auto operator()(const ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val,std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.time_date,val.value);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        auto operator()(ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val,std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.time_date,val.value);
        }
    };

    template<>
    struct Serial_size<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        auto operator()(const ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val) const noexcept{
            return serial_size(val.time_date,val.value);
        }
    };

    template<>
    struct Min_serial_size<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>::time_date),decltype(ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>::value)>();
        }();
    };

    template<>
    struct Max_serial_size<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>::time_date),decltype(ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>::value)>();
        }();
    };
}

#include "boost_functional/json.h"

template<>
boost::json::value to_json(const ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val);

template<>
std::expected<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& vals);
