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
    EXTRACTION_TIME_INTERVAL,
    CAPITALIZE_HIERARCHY,
    CAPITALIZE_FORMAT,
    CENTER,
    PARAMETERS,
    TABLE_VERSION,
    COLLECTION,
    TIME_FCST,
    GRID_TYPE
};

enum class ModeArgs:uint8_t{
    NONE,
    EXTRACT,
    CAPITALIZE,
    INTEGRITY,
    CONFIG,
    HELP,
    SAVE,
    EXIT,
    CONTAINS,
    SERVER
};
enum class ServerConfigCommands:uint8_t{
    NAME,
    HOST,
    PORT,
    SERVICE,
    ACCEPTED_ADDRESSES,
    TIMEOUT,
    PROTOCOL
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
enum class ServerAction:uint8_t{
    ADD,
    SET,
    REMOVE,
    LAUNCH,
    SHUTDOWN,
    CLOSE,
    COLLAPSE,
    GET_CURRENT_CONFIG
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
struct __Token_text__<translate::token::ServerAction>{
    static constexpr std::array<const char*,8> txt = {
        "add",
        "set",
        "remove",
        "launch",
        "shutdown",
        "close",
        "collapse",
        "getcur"
    };
};

template<>
struct __Token_text__<translate::token::ServerConfigCommands>{
    static constexpr std::array<const char*,7> txt = {
        "-name",
        "-host",
        "-port",
        "-service",
        "-accaddr",
        "-timeout",
        "-protocol"
    };
};

template<>
struct __Token_text__<translate::token::Command>{
    static constexpr std::array<const char*,21> txt = {
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
        "-extoutfmt",
        "-extti",
        "-caporder",
        "-capfmt",
        "-center",
        "-params",
        "-tablev",
        "-collect",
        "-time_fcst",
        "-grid"
    };
};

template<>
struct __Token_text__<translate::token::ModeArgs>{
    static constexpr std::array<const char*,10> txt = {
        "",
        "-ext",
        "-cap",
        "-integrity",
        "-config",
        "-help",
        "-save",
        "exit()",
        "-contains",
        "-server"
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
    return (T)-1;
}

template<typename T>
inline std::string_view translate_from_token(T token) noexcept{
    return __Token_text__<T>::txt.at((uint)token);
}