#pragma once
#include <cinttypes>
#include <string>
#include <vector>
#include <string_view>
#include <array>

enum class Command:uint16_t{
    UNDEF,
    OUT_PATH,
    IN_PATH,
    DATE_FROM,
    DATE_TO,
    POSITION,
    LAT_TOP,
    LON_LEFT,
    LAT_BOT,
    LON_RIG,
    THREADS,
    EXTRACT_FORMAT,
    EXTRACTION_DIV,
    CAPITALIZE_HIERARCHY,
    CAPITILIZE_FORMAT
};

enum class ExtractFormatArgs:uint8_t{
    UNDEF,
    TXT,
    BIN,
    GRIB,
    ARCHIVED
};

enum class ExtractDivArgs:uint8_t{
    UNDEF,
    HOUR,
    MONTH,
    DAY,
    LAT,
    LON,
    LATLON
};

enum class CapitalizeHierArgs:uint8_t{
    UNDEF,
    HOUR,
    MONTH,
    YEAR,
    DAY,
    LAT,
    LON,
    LATLON
};

enum class CapitilizeFormatArgs:uint8_t{
    UNDEF,
    TXT,
    BIN,
    GRIB
};

constexpr std::array<const char*,8> extract_div_txt = {
    "",
    "h",
    "m",
    "d",
    "y",
    "lat",
    "lon",
    "latlon"
};

constexpr std::array<const char*,8> capitalize_hier_txt = {
    "",
    "h",
    "m",
    "d",
    "y",
    "lat",
    "lon",
    "latlon"
};

constexpr std::array<const char*,4> capitalize_fmt_txt = {
    "",
    "txt",
    "bin",
    "grib"
};

constexpr std::array<const char*,5> extract_fmt_txt = {
    "",
    "txt",
    "bin",
    "grib",
    "zip"
};

constexpr std::array<const char*,15> cmd_txt = {
    "",
    "-outp",
    "-inp",
    "-dfrom",
    "-dto",
    "-pos",
    "-lattop",
    "-latbot",
    "-lonleft",
    "-lonrig",
    "-j",
    "-extfmt",
    "-divby",
    "-hier",
    "-format"
};

/*
enum DATA_OUT{
    DEFAULT= 0,
    TXT_F = 1,
    BIN_F = 2,
    GRIB_F = 3<<1,
    ARCHIVED = 4<<2
};

//separation by files
enum DIV_DATA_OUT{
    ALL_IN_ONE = 0, //all in one file with data inline
    YEAR_T = 1<<0,
    MONTH_T = 1<<2,
    DAY_T = 1<<3,
    HOUR_T = 1<<4,
    LAT = 1<<5,
    LON = 1<<6
};
*/

Command translate_from_txt(std::string_view cmd);
ExtractFormatArgs translate_extract_fmt(std::string_view arg);
ExtractDivArgs translate_extract_div(std::string_view arg);
CapitalizeHierArgs translate_cap_hierarchy(std::string_view arg);
CapitilizeFormatArgs translate_cap_format(std::string_view arg);
