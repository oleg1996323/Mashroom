#pragma once
#include <array>
#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>

constexpr std::string_view bindata_filename = std::string_view("data");

struct __Data__{
    //@todo inherit from API enum
    enum class FORMAT{
        UNDEF,
        GRIB_v1
        /* HGT,
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
    /* __Data__::FORMAT::HGT,
    __Data__::FORMAT::NETCDF */
};

__Data__::FORMAT text_to_data_type(std::string_view text) noexcept;

constexpr std::array<std::string_view,2> data_extensions ={
    std::string_view(""),     //undefined
    std::string_view(".gbd")  //grib binary data
};

static const std::unordered_map<const char*,Data_f> formats = {
                    {"undef",Data_f::UNDEF},
                    {"grib_v1",Data_f::GRIB_v1}};
                    /* {"hgt",Data_f::HGT},
                    {"netCDF",Data_f::NETCDF} };*/

__Data__::FORMAT extension_to_type(std::string_view extension) noexcept;
constexpr const char* format_name(__Data__::FORMAT fmt) noexcept{
    switch(fmt){
        case __Data__::FORMAT::GRIB_v1:
            return "Grib";
        // case __Data__::FORMAT::HGT:
        //     return "HGT";
        // case __Data__::FORMAT::NETCDF:
        //     return "NetCDF";
        default:
            return "Undefined";
    }
}
std::string_view type_to_extension(__Data__::FORMAT type_extension) noexcept;
std::string filename_by_type(__Data__::FORMAT type);