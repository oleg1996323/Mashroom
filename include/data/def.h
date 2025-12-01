#pragma once
#include <array>
#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>
#include <optional>
#include "sys/error_code.h"
#include <unordered_set>
#include <vector>

constexpr std::string_view bindata_filename = std::string_view("data");

struct __Data__{
    //@todo inherit from API enum
    enum class FORMAT{
        GRIB_v1
        
        /* 
        GRIB_v2
        GRIB_v3
        HGT,
        NETCDF */
    };

    enum class ACCESS:uint8_t{
        PUBLIC,
        PROTECTED,
        PRIVATE
    };

    enum class TYPE:uint8_t{
        TIME_SERIES = 0,
        GRID,
        POLYGONE,
        ISOLINES
    };
};

using Data_t = __Data__::TYPE;
using Data_f = __Data__::FORMAT;
using Data_a = __Data__::ACCESS;

std::size_t number_of_formats() noexcept;
std::size_t number_of_types() noexcept;
std::optional<std::vector<Data_f>> extension_to_tokens(std::string_view extension) noexcept;
const std::vector<std::string_view>& token_to_extensions(Data_f token) noexcept;
std::string filename_by_format(Data_f token);
std::string_view preferred_extension(Data_f format);
std::optional<Data_f> to_data_format_token(std::string_view text) noexcept;
std::optional<Data_t> to_data_type_token(std::string_view text) noexcept;
std::string_view to_data_format_name(Data_f token) noexcept;
std::string_view to_data_type_name(Data_t token) noexcept;