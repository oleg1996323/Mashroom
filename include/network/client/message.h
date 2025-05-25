#pragma once
#include <optional>
#include <network/common/def.h>
#include <extract.h>
#include <network/common/message_handler.h>

using namespace std::chrono;
namespace fs = std::filesystem;
namespace network::client{
enum class TYPE_MESSAGE : uint8_t{
    DATA_REQUEST,
    SERVER_STATUS,
    CAPITALIZE
};

template<TYPE_MESSAGE>
struct Message;

template<>
struct Message<TYPE_MESSAGE::DATA_REQUEST>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::DATA_REQUEST;
    //further add field that modify type of meteo (grib/netcdf/rp5 etc.)
    std::optional<TimeFrame> t_fcst_;
    std::optional<uint16_t> parameter_sz_;
    std::optional<Organization> center;
    std::optional<utc_tp> from;   //requested from Date
    std::optional<utc_tp> to;     //requested to Date
    std::optional<Coord> pos;   //requested position (in WGS)
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
struct Message<TYPE_MESSAGE::SERVER_STATUS>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::SERVER_STATUS;
    bool sendto(int sock);
};
template<>
struct Message<TYPE_MESSAGE::CAPITALIZE>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::CAPITALIZE;
};
constexpr std::array<size_t,3> sizes_msg_struct={
    sizeof(Message<TYPE_MESSAGE::DATA_REQUEST>),
    sizeof(Message<TYPE_MESSAGE::SERVER_STATUS>),
    sizeof(Message<TYPE_MESSAGE::CAPITALIZE>)
};
constexpr size_t num_msg = sizes_msg_struct.size();
using request_message = std::variant<std::monostate,
                        network::client::Message<TYPE_MESSAGE::DATA_REQUEST>,
                        network::client::Message<TYPE_MESSAGE::SERVER_STATUS>,
                        network::client::Message<TYPE_MESSAGE::CAPITALIZE>>;

namespace detail{
class MessageHandler:public network::detail::MessageHandler<TYPE_MESSAGE,request_message>{};
}

template<typename MSG_T>
concept MessageType_concept = requires(MSG_T msg_t){
    std::is_enum_v<MSG_T>;
    requires std::is_same_v<network::client::TYPE_MESSAGE,MSG_T>;
};

class MessageProcess:public detail::MessageHandler{
    public:
    MessageProcess(){
        network::detail::check_variant_enum<TYPE_MESSAGE,request_message,num_msg>();
    }
};
}