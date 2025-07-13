#pragma once
#include <array>
#include <string>
#include <string_view>

constexpr std::string_view bindata_filename = std::string_view("data");

struct __Data__{
    enum class FORMAT{
        UNDEF,
        GRIB,
        HGT,
        NETCDF
    };
};

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
const char* type_to_extension(__Data__::FORMAT type_extension) noexcept;
const char* data_type_to_text(__Data__::FORMAT type) noexcept;
std::string filename_by_type(__Data__::FORMAT type);