#pragma once
#include <optional>
#include <network/common/def.h>
#include <network/common/message_handler.h>
#include <extract.h>

using namespace std::chrono;
namespace fs = std::filesystem;
namespace server{
enum class TYPE_MESSAGE : uint8_t{
    METEO_REPLY,
    SERVER_CHECK,
    CHECK_DATA,
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
    server::Status status_=server::Status::READY;
};
template<>
struct Message<TYPE_MESSAGE::METEO_REPLY>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::METEO_REPLY;
    MessageFileInfo finfo_;
    size_t total_sz_=sizeof(total_sz_)+sizeof(finfo_);
    ProgressBase progress_;
    server::Status status_=server::Status::READY;
    bool sendto(int sock,const fs::path& file_send);
};
template<>
struct Message<TYPE_MESSAGE::SERVER_CHECK>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::SERVER_CHECK;
    server::Status status_=server::Status::READY;
    bool sendto(int sock);
};
template<>
struct Message<TYPE_MESSAGE::ERROR>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::ERROR;
    std::array<char,256> error_message;
    ErrorCode err_ = ErrorCode::NONE;
    server::Status status_=server::Status::READY;
    bool sendto(int sock,ErrorCode err, std::string msg);
};
template<>
struct Message<TYPE_MESSAGE::CHECK_DATA>{
    static constexpr TYPE_MESSAGE type_msg_ = TYPE_MESSAGE::CHECK_DATA;
    server::Status status_=server::Status::READY;
    //TODO: set structure for reply message
    //maybe add data-type (topo,meteo,kadasr etc)
};
constexpr std::array<size_t,5> sizes_msg_struct={
    sizeof(Message<TYPE_MESSAGE::METEO_REPLY>),
    sizeof(Message<TYPE_MESSAGE::SERVER_CHECK>),
    sizeof(Message<TYPE_MESSAGE::CHECK_DATA>),
    sizeof(Message<TYPE_MESSAGE::ERROR>),
    sizeof(Message<TYPE_MESSAGE::PROGRESS>)
};
constexpr size_t num_msg = sizes_msg_struct.size();
using reply_message = std::variant<std::monostate,
                        server::Message<TYPE_MESSAGE::METEO_REPLY>,
                        server::Message<TYPE_MESSAGE::SERVER_CHECK>,
                        server::Message<TYPE_MESSAGE::CHECK_DATA>,
                        server::Message<TYPE_MESSAGE::ERROR>,
                        server::Message<TYPE_MESSAGE::PROGRESS>>;

class MessageProcess:public ::network::MessageHandler<::server::TYPE_MESSAGE,reply_message>{
    public:
    MessageProcess(){
        ::network::check_variant_enum<TYPE_MESSAGE,reply_message,num_msg>();
    }
};
}