#pragma once
#include <variant>
#include <cstdint>
#include <cstddef>
#include "types/time_interval.h"
#include "API/grib1/include/properties.h"
#include "serialization.h"
#include <optional>
#include "web/common/msgdef.h"
#include "data/def.h"
#include "sys/outputdatafileformats.h"

template <Data_t T, Data_f F>
struct ExtractRequestForm;

template <>
struct ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1>
{
    SearchProperties search_props_;
    std::optional<DateTimeDiff> t_separation_;
    std::optional<OutputDataFileFormats> file_fmt_;
    ExtractRequestForm() = default;
    explicit ExtractRequestForm(const SearchProperties& search_props,
        std::optional<DateTimeDiff> t_sep,
        std::optional<OutputDataFileFormats> file_fmt);
    explicit ExtractRequestForm(SearchProperties&& search_props,
        std::optional<DateTimeDiff>&& t_sep,
        std::optional<OutputDataFileFormats> file_fmt);
    ExtractRequestForm(const ExtractRequestForm& other) = delete;
    ExtractRequestForm(ExtractRequestForm&& other) noexcept;
    ExtractRequestForm& operator=(const ExtractRequestForm& other) = delete;
    ExtractRequestForm& operator=(ExtractRequestForm&& other) noexcept;
};

using ExtractMeteoGrib = ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1>;

namespace serialization
{   
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,ExtractMeteoGrib>{
        auto operator()(const ExtractMeteoGrib& val,std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.search_props_,val.t_separation_,val.file_fmt_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,ExtractMeteoGrib>{
        auto operator()(ExtractMeteoGrib& val,StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.search_props_,val.t_separation_,val.file_fmt_);
        }
    };

    template<>
    struct Serial_size<ExtractMeteoGrib>{
        auto operator()(const ExtractMeteoGrib& val) const noexcept{
            return serial_size(val.search_props_,val.t_separation_,val.file_fmt_);
        }
    };

    template<>
    struct Min_serial_size<ExtractMeteoGrib>{
        using type = ExtractMeteoGrib;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::search_props_),decltype(type::t_separation_),decltype(type::file_fmt_)>();
        }();
    };

    template<>
    struct Max_serial_size<ExtractMeteoGrib>{
        using type = ExtractMeteoGrib;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::search_props_),decltype(type::t_separation_),decltype(type::file_fmt_)>();
        }();
    };
}

using ExtractForm = std::variant<std::monostate,ExtractMeteoGrib>;