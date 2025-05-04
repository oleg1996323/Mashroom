#pragma once
#include <optional>
#include <network/def.h>
#include <extract.h>

using namespace std::chrono;
namespace fs = std::filesystem;
namespace network{
enum class TYPE_MESSAGE : uint8_t{
    NONE = 0,
    METEO_REQUEST,
    METEO_REPLY,
    SERVER_CHECK,
    CHECK_DATA,
    ERROR
};
template<TYPE_MESSAGE>
struct Message;

template<>
struct Message<TYPE_MESSAGE::METEO_REQUEST>{
    const TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::METEO_REQUEST;
    //further add field that modify type of meteo (grib/netcdf/rp5 etc.)
    std::optional<TimeFrame> t_fcst_;
    std::optional<uint16_t> parameter_sz_;
    std::optional<Organization> center;
    std::optional<utc_tp> from;   //requested from Date
    std::optional<utc_tp> to;     //requested to Date
    std::optional<Coord> pos;   //requested position
    std::optional<RepresentationType> rep_t;
    std::optional<TimeOffset> time_off_;
    std::optional<Extract::ExtractFormat> fmt_ = Extract::ExtractFormat::TXT_F|Extract::ExtractFormat::ARCHIVED;
    Extract prepare_and_check_integrity_extractor(ErrorCode& err) const;
};
struct MessageFileInfo{
    char filename[256];
    uintmax_t data_sz = 0;      //size of file
};
template<>
struct Message<TYPE_MESSAGE::METEO_REPLY>{
    const TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::METEO_REPLY;
    MessageFileInfo finfo_;
    size_t total_sz_=sizeof(total_sz_)+sizeof(finfo_);
    bool sendto(int sock,const fs::path& file_send,bool& interrupt);
};
template<>
struct Message<TYPE_MESSAGE::SERVER_CHECK>{
    const TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::SERVER_CHECK;
    server::Status status = server::Status::INACTIVE;
    bool sendto(int sock);
};
template<>
struct Message<TYPE_MESSAGE::ERROR>{
    const TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::ERROR;
    std::array<char,256> error_message;
    ErrorCode err_ = ErrorCode::NONE;
    bool sendto(int sock,ErrorCode err, std::string msg);
};
template<>
struct Message<TYPE_MESSAGE::CHECK_DATA>{
    const TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::CHECK_DATA;
    utc_tp from = {};
    utc_tp to = {};
    Coord lefttop = {};
    Coord rightbottom = {};
};
constexpr std::array<size_t,6> sizes_msg_struct={
    sizeof(0),
    sizeof(Message<TYPE_MESSAGE::METEO_REQUEST>),
    sizeof(Message<TYPE_MESSAGE::METEO_REPLY>),
    sizeof(Message<TYPE_MESSAGE::SERVER_CHECK>),
    sizeof(Message<TYPE_MESSAGE::CHECK_DATA>),
    sizeof(Message<TYPE_MESSAGE::ERROR>)
};
}