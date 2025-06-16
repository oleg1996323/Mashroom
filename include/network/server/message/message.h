#pragma once
#include <network/server/message/capitalize.h>
#include <network/server/message/data_reply_finfo.h>
#include <network/server/message/data_reply_fpart.h>
#include <network/server/message/data_reply_cap_info.h>
#include <network/server/message/data_reply_extract_part.h>
#include <network/server/message/error.h>
#include <network/server/message/progress.h>
#include <network/server/message/status.h>
#include <network/server/message/version.h>

#include <optional>
#include <network/common/def.h>
#include <extract.h>
#include <program/data.h>
using namespace std::chrono;
namespace fs = std::filesystem;
namespace network::server{

using reply_message = std::variant<std::monostate,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>,
                        server::Message<TYPE_MESSAGE::SERVER_STATUS>,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE>,
                        server::Message<TYPE_MESSAGE::ERROR>,
                        server::Message<TYPE_MESSAGE::PROGRESS>,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>,
                        server::Message<TYPE_MESSAGE::VERSION>,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE_REF>,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_EXTRACT>>;

class MessageProcess;
namespace detail{
using associated_t = std::variant<std::monostate,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>,
                        server::Message<TYPE_MESSAGE::SERVER_STATUS>,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE>,
                        server::Message<TYPE_MESSAGE::ERROR>,
                        server::Message<TYPE_MESSAGE::PROGRESS>,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>::associated_t,
                        server::Message<TYPE_MESSAGE::VERSION>,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE_REF>,
                        server::Message<TYPE_MESSAGE::DATA_REPLY_EXTRACT>>;

class MessageHandler:public network::detail::MessageHandler<TYPE_MESSAGE,reply_message>{
    private:
    using network::detail::MessageHandler<TYPE_MESSAGE,reply_message>::MessageHandler;
    friend class network::server::MessageProcess;
    std::unique_ptr<associated_t> msg_data_;
    ErrorCode err_;
    template<TYPE_MESSAGE MSG,typename... ARGS>
    bool create_message(ARGS&&... args){
        msg_data_ = std::make_unique<Message<MSG>::associated_t>();
        *this = Message<MSG>(*msg_data_.get(),std::forward<ARGS>(args...));
        err_ = msg_data_->err_;
    }
};
}

class MessageProcess{
    static_assert(network::detail::check_variant_enum<TYPE_MESSAGE,reply_message,std::variant_size_v<reply_message>-1>);
    network::server::detail::MessageHandler hmsg_;
    ErrorCode err_;
    bool __deserialize_msg__(std::vector<char>&& buffer);
    bool __receive_buffer__(int sock, std::vector<char>& buffer);
    public:
    bool receive(int sock){
        std::vector<char> buffer;
        if(__receive_buffer__(sock,buffer))
            if(__deserialize_msg__(std::move(buffer)))
                return true;
        return false;
    }

    std::optional<TYPE_MESSAGE> get_msg_type() const{
        if(std::holds_alternative<std::monostate>(hmsg_))
            return std::nullopt;
        else return (TYPE_MESSAGE)hmsg_.index();
    }

    bool send_file_by_parts(int sock,network::server::Status status,uint32_t chunk,const fs::path& filename);
    bool send_error(int sock,network::server::Status status,ErrorCode err);
};

/// @brief 
/// @details Send header with file information. If accepted by client, the file will be sent by parts.
/// @param sock socket
/// @param status server status
/// @param chunk part of file data by message
/// @param path path destination of file
/// @return success or failure of sending
bool MessageProcess::send_file_by_parts(int sock,network::server::Status status,uint32_t chunk,const fs::path& path){
    if(!hmsg_.create_message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>(path,status))
        return false;
    auto& msg = hmsg_.get<Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>>();
    Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>::serialize(msg);
    int16_t sent = 0;
    while(sent<=msg.buffer().size()-1){
        sent = send(sock,msg.buffer().data()+sent,msg.buffer().size(),0);
        if(sent<0 || errno!=0){
            err_ = ErrorPrint::print_error(ErrorCode::SENDING_REPLY_ERROR,strerror(errno),AT_ERROR_ACTION::CONTINUE);
            if(!send_error(sock,Status::READY,err_))
                close(sock);
            return false;
        }
    }
    MessageProcess accept_decline;
    /// @todo (ACCEPT or DECLINE)
    accept_decline.receive(sock);
}
/// @brief 
/// @param sock socket
/// @param status server status
/// @param err error code
/// @return 
bool MessageProcess::send_error(int sock,network::server::Status status,ErrorCode err){
    return false;
}
}