#pragma once
#include <optional>
#include <network/common/def.h>
#include <network/common/message_handler.h>
#include <extract.h>
#include <program/data.h>
using namespace std::chrono;
namespace fs = std::filesystem;
namespace network::server{
enum class TYPE_MESSAGE : uint8_t{
    METEO_REPLY,
    SERVER_STATUS,
    CAPITALIZE,
    ERROR,
    PROGRESS
};
template<TYPE_MESSAGE>
struct Message;

//any server message send in same time its status by DefaultMessage struct
struct MessageFileInfo{
    char filename[256];
    uintmax_t data_sz = 0;      //size of file
};
struct ProgressBase{
    enum struct Process:uint8_t{
        NOTHING,
        SEARCH,
        EXTRACT,
        PREPARE,
        SENDING
    };
    float progress_=0;
    Process server_process_ = Process::NOTHING;
};
template<>
struct Message<TYPE_MESSAGE::PROGRESS>:ProgressBase{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::PROGRESS;
    ProgressBase prog_;
    network::server::Status status_=network::server::Status::READY;
};
template<>
struct Message<TYPE_MESSAGE::METEO_REPLY>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::METEO_REPLY;
    MessageFileInfo finfo_;
    size_t total_sz_=sizeof(total_sz_)+sizeof(finfo_);
    ProgressBase progress_;
    network::server::Status status_=network::server::Status::READY;
    bool sendto(int sock,const fs::path& file_send);
};
template<>
struct Message<TYPE_MESSAGE::SERVER_STATUS>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::SERVER_STATUS;
    network::server::Status status_=network::server::Status::READY;
    bool sendto(int sock);
};
template<>
struct Message<TYPE_MESSAGE::ERROR>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::ERROR;
    std::array<char,256> error_message;
    ErrorCode err_ = ErrorCode::NONE;
    network::server::Status status_=network::server::Status::READY;
    bool sendto(int sock,ErrorCode err, std::string msg);
};

template<>
struct Message<TYPE_MESSAGE::CAPITALIZE>{
    template<Data::TYPE T,Data::FORMAT F>
    struct CapitalizeResultProxy{
        static constexpr Data::TYPE data_type = T;
        static constexpr Data::FORMAT data_format = F;
    };
    template<Data::TYPE,Data::FORMAT>
    struct CapitalizeResult;

    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::CAPITALIZE;
    network::server::Status status_=network::server::Status::READY;
    size_t msg_sz_ = 0;
    
    //TODO: set structure for reply message
    //maybe add data-type (topo,meteo,kadasr etc)
    bool sendto(int sock);
};
template<>
struct Message<TYPE_MESSAGE::CAPITALIZE>::CapitalizeResult<Data::TYPE::METEO,Data::FORMAT::GRIB>:
Message<TYPE_MESSAGE::CAPITALIZE>::CapitalizeResultProxy<Data::TYPE::METEO,Data::FORMAT::GRIB>
{
    size_t common_data_number_;
    CommonDataProperties common_data_;
    //TODO: complete
};
constexpr std::array<size_t,5> sizes_msg_struct={
    sizeof(Message<TYPE_MESSAGE::METEO_REPLY>),
    sizeof(Message<TYPE_MESSAGE::SERVER_STATUS>),
    sizeof(Message<TYPE_MESSAGE::CAPITALIZE>),
    sizeof(Message<TYPE_MESSAGE::ERROR>),
    sizeof(Message<TYPE_MESSAGE::PROGRESS>)
};
constexpr size_t num_msg = sizes_msg_struct.size();
using reply_message = std::variant<std::monostate,
                        server::Message<TYPE_MESSAGE::METEO_REPLY>,
                        server::Message<TYPE_MESSAGE::SERVER_STATUS>,
                        server::Message<TYPE_MESSAGE::CAPITALIZE>,
                        server::Message<TYPE_MESSAGE::ERROR>,
                        server::Message<TYPE_MESSAGE::PROGRESS>>;

namespace detail{
class MessageHandler:public network::detail::MessageHandler<TYPE_MESSAGE,reply_message>{};
}

class MessageProcess:public detail::MessageHandler{
    public:
    MessageProcess(){
        network::detail::check_variant_enum<TYPE_MESSAGE,reply_message,num_msg>();
    }
};
}