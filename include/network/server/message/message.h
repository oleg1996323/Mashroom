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
#include <network/server/message/data_transaction.h>

#include <optional>
#include <network/common/def.h>
#include <network/common/message_handler.h>
#include <extract.h>
#include <program/data.h>
using namespace std::chrono;
namespace fs = std::filesystem;
namespace network::server{
// constexpr std::array<size_t,9> sizes_msg_struct={
//     sizeof(Message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>),
//     sizeof(Message<TYPE_MESSAGE::SERVER_STATUS>),
//     sizeof(Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE>),
//     sizeof(Message<TYPE_MESSAGE::ERROR>),
//     sizeof(Message<TYPE_MESSAGE::PROGRESS>),
//     sizeof(Message<TYPE_MESSAGE::DATA_REPLY_FILEPART>),
//     sizeof(Message<TYPE_MESSAGE::VERSION>),
//     sizeof(Message<TYPE_MESSAGE::DATA_REPLY_CAPITALIZE_REF>),
//     sizeof(Message<TYPE_MESSAGE::DATA_REPLY_EXTRACT>)
// };
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

constexpr size_t num_msg = std::variant_size_v<reply_message>-1;
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
        *this = Message<MSG>(*msg_data_.get(),std::forward<ARGS>(args));
        err_ = msg_data_->err_;
    }
};
}

class MessageProcess{
    static_assert(network::detail::check_variant_enum<TYPE_MESSAGE,reply_message,num_msg>());
    network::server::detail::MessageHandler hmsg_;
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
};

bool MessageProcess::send_file_by_parts(int sock,network::server::Status status,uint32_t chunk,const fs::path& path){
    if(!hmsg_.create_message<TYPE_MESSAGE::DATA_REPLY_FILEINFO>(path,status))
        return false;
    std::vector<char> to_send_buf = Message<MSG>::serialize(msg);
    msg::send_impl(sock,msg);
}
}