#pragma once
#include <cinttypes>
#include <string>
#include <vector>
#include <string_view>
#include <array>

namespace translate::token{

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
    CAPITALIZE_FORMAT
};

enum class ModeArgs:uint8_t{
    NONE,
    EXTRACT,
    CAPITALIZE,
    CHECK,
    CONFIG,
    HELP
};

enum class ConfigAction:uint8_t{
    NONE,
    ADD,
    DELETE,
    GET_COMMAND
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
}

template<typename T>
struct __Token_text__;

template<>
struct __Token_text__<translate::token::Command>{
    static constexpr std::array<const char*,15> txt = {
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
};

template<>
struct __Token_text__<translate::token::ModeArgs>{
    static constexpr std::array<const char*,6> txt = {
        "",
        "-ext",
        "-cap",
        "-check",
        "-config",
        "-help"
    };
};

template<>
struct __Token_text__<translate::token::ConfigAction>{
    static constexpr std::array<const char*,4> txt = {
        "",
        "add",
        "delete",
        "get_command"
    };
};

template<>
struct __Token_text__<translate::token::CapitalizeHierArgs>{
    static constexpr std::array<const char*,8> txt = {
        "",
        "h",
        "m",
        "d",
        "y",
        "lat",
        "lon",
        "latlon"
    };
};

template<>
struct __Token_text__<translate::token::ExtractDivArgs>{
    static constexpr std::array<const char*,8> txt = {
        "",
        "h",
        "m",
        "d",
        "y",
        "lat",
        "lon",
        "latlon"
    };
};

template<>
struct __Token_text__<translate::token::CapitilizeFormatArgs>{
    static constexpr std::array<const char*,4> txt = {
        "",
        "txt",
        "bin",
        "grib"
    };
};

template<>
struct __Token_text__<translate::token::ExtractFormatArgs>{
    static constexpr std::array<const char*,5> txt = {
        "",
        "txt",
        "bin",
        "grib",
        "zip"
    };
};
template<typename T>
inline T translate_from_txt(std::string_view cmd) noexcept{
    for(int i=0;i<__Token_text__<T>::txt.size();++i){
        if(cmd==__Token_text__<T>::txt[i])
            return (T)i;
    }
    return (T)0U;
}

template<typename T>
inline std::string_view translate_from_token(T token) noexcept{
    return __Token_text__<T>::txt.at((uint)token);
}