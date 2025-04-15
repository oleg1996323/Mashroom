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
    UNDEF,      //undefined action (error)
    ADD,        //check and add configuration by name
    REDEFINE,   //redefine configuration by name if exists or create
    REMOVE,     //remove configuration by name
    SET,        //set permanent configuration
    ADDSET,
    GET         //get config action by name or all (if '.' is inputed)
};    

enum class FileFormat:uint8_t{
    UNDEF,
    TXT,
    BIN,
    GRIB,
    ARCHIVED
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
    static constexpr std::array<const char*,7> txt = {
        "",
        "add",
        "redef",
        "remove",
        "set",
        "addset",
        "get"
    };
};

template<>
struct __Token_text__<translate::token::FileFormat>{
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