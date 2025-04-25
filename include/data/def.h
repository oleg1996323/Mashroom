#pragma once
#include <array>
#include <string>

constexpr std::string_view bindata_filename = "data";

enum class DataTypeInfo{
    Undef,
    Grib
};

constexpr std::array<DataTypeInfo,1> data_types = {
    DataTypeInfo::Grib
};

DataTypeInfo text_to_data_type(const char* text) noexcept;

constexpr std::array<const char*,2> data_extensions ={
    "",     //undefined
    ".gbd"  //grib binary data
};

DataTypeInfo extension_to_type(const char* extension) noexcept;
const char* type_to_extension(DataTypeInfo type_extension) noexcept;
const char* data_type_to_text(DataTypeInfo type) noexcept;
std::string filename_by_type(DataTypeInfo type);