#pragma once
#include <array>
#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>
#include <optional>
#include "sys/error_code.h"
#include <unordered_set>

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
        METEO,
        TOPO,
        KADASTR
    };
};

using Data_t = __Data__::TYPE;
using Data_f = __Data__::FORMAT;
using Data_a = __Data__::ACCESS;

constexpr std::array<__Data__::FORMAT,1> data_types = {
    __Data__::FORMAT::GRIB_v1,
    /* 
    GRIB_v2
    GRIB_v3
    __Data__::FORMAT::HGT,
    __Data__::FORMAT::NETCDF */
};

std::optional<std::vector<Data_f>> extension_to_tokens(std::string_view extension) noexcept;
std::optional<__Data__::FORMAT> text_to_data_format(std::string_view text) noexcept;
const std::vector<std::string_view>& token_to_extensions(__Data__::FORMAT type_extension) noexcept;
std::string filename_by_type(__Data__::FORMAT type);
std::string_view preferred_extension(Data_f format);