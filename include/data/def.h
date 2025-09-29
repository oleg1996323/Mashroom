#pragma once
#include <array>
#include <string>
#include <string_view>
#include <cstdint>

constexpr std::string_view bindata_filename = std::string_view("data");

struct __Data__{
    enum class FORMAT{
        UNDEF,
        GRIB,
        HGT,
        NETCDF
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

constexpr std::array<__Data__::FORMAT,3> data_types = {
    __Data__::FORMAT::GRIB,
    __Data__::FORMAT::HGT,
    __Data__::FORMAT::NETCDF
};

__Data__::FORMAT text_to_data_type(const char* text) noexcept;

constexpr std::array<const char*,2> data_extensions ={
    "",     //undefined
    ".gbd"  //grib binary data
};

__Data__::FORMAT extension_to_type(const char* extension) noexcept;
constexpr const char* format_name(__Data__::FORMAT fmt) noexcept{
    switch(fmt){
        case __Data__::FORMAT::GRIB:
            return "Grib";
        case __Data__::FORMAT::HGT:
            return "HGT";
        case __Data__::FORMAT::NETCDF:
            return "NetCDF";
        default:
            return "Undefined";
    }
}
const char* type_to_extension(__Data__::FORMAT type_extension) noexcept;
std::string filename_by_type(__Data__::FORMAT type);