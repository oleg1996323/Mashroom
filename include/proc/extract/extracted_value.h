#pragma once
#include "data/def.h"
#include "byte_order.h"
#include "types/time_interval.h"
#include <variant>

template<Data_t T,Data_f F>
struct ExtractedValue;

template<>
struct ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>
{
    std::chrono::system_clock::time_point time_date;
    float value = UNDEFINED;

    ExtractedValue() = default;
    ExtractedValue(utc_tp time, float val) : time_date(time), value(val) {}
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

#include "data/common_data_properties.h"
template<Data_t T,Data_f F>
using ExtractedValues = std::unordered_map<CommonDataProperties<T,F>, std::vector<ExtractedValue<T,F>>>;

using VariantExtractedValue = std::variant<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;
using VariantExtractedData = std::variant<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;
using ExtractedData = VariantExtractedData;

static_assert(std::is_default_constructible_v<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>>);
static_assert(std::is_default_constructible_v<VariantExtractedData>);