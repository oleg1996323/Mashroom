#pragma once
#include <cinttypes>
#include <variant>
#include <network/common/message/message_handler.h>
#include <network/common/message/msgdef.h>
#include <optional>
#include <network/common/def.h>
#include <extract.h>
#include <network/client/message/capitalize.h>
#include <network/client/message/capitalize_ref.h>
#include <network/client/message/status.h>
#include <network/client/message/data_request.h>
#include <network/client/message/transaction.h>

#include <types/time_interval.h>
#include <sys/error_code.h>

using namespace std::chrono;
namespace fs = std::filesystem;
namespace network::client{

using request_message = std::variant<std::monostate,
                        Message<TYPE_MESSAGE::DATA_REQUEST>,
                        Message<TYPE_MESSAGE::SERVER_STATUS>,
                        Message<TYPE_MESSAGE::CAPITALIZE>,
                        Message<TYPE_MESSAGE::CAPITALIZE_REF>,
                        Message<TYPE_MESSAGE::TRANSACTION>>;

class MessageProcess;
namespace detail{
using associated_t = std::variant<std::monostate,
                        AssociatedData<TYPE_MESSAGE::DATA_REQUEST>,
                        AssociatedData<TYPE_MESSAGE::SERVER_STATUS>,
                        AssociatedData<TYPE_MESSAGE::CAPITALIZE>,
                        AssociatedData<TYPE_MESSAGE::CAPITALIZE_REF>,
                        AssociatedData<TYPE_MESSAGE::TRANSACTION>>;

class MessageHandler:public network::detail::MessageHandler<TYPE_MESSAGE,request_message>{
    private:
    using network::detail::MessageHandler<TYPE_MESSAGE,request_message>::MessageHandler;
    friend class network::client::MessageProcess;
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
    static_assert(network::detail::check_variant_enum<TYPE_MESSAGE,request_message,std::variant_size_v<request_message>-1>);
    network::client::detail::MessageHandler hmsg_;
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
}