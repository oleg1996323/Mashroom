#pragma once
#include <network/common/message/msgdef.h>

namespace network{
template<>
struct MessageAdditional<network::Server_MsgT::ERROR>{
    server::Status status_;
    ErrorCode err_ = ErrorCode::NONE;
    MessageAdditional(    ErrorCode& err,ErrorCode error_code,
                server::Status status = server::Status::READY){}
    MessageAdditional() = default;
    MessageAdditional& operator=(const MessageAdditional& other) = default;
    MessageAdditional& operator=(MessageAdditional&& other) noexcept = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::ERROR>>{
        using type = MessageAdditional<network::Server_MsgT::ERROR>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.err_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::ERROR>>{
        using type = MessageAdditional<network::Server_MsgT::ERROR>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.err_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Server_MsgT::ERROR>>{
        using type = MessageAdditional<network::Server_MsgT::ERROR>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.status_,msg.err_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Server_MsgT::ERROR>>{
        using type = MessageAdditional<network::Server_MsgT::ERROR>;
        size_t operator()(const type& msg) const noexcept{
            return min_serial_size(msg.status_,msg.err_);
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Server_MsgT::ERROR>>{
        using type = MessageAdditional<network::Server_MsgT::ERROR>;
        size_t operator()(const type& msg) const noexcept{
            return max_serial_size(msg.status_,msg.err_);
        }
    };
}