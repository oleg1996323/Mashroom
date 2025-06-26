#pragma once
#include <network/common/message/msgdef.h>

namespace network{
template<>
struct MessageAdditional<Server_MsgT::SERVER_STATUS>{
    server::Status status_ = server::Status::READY;
    MessageAdditional(ErrorCode& err,server::Status status){}
    MessageAdditional(const MessageAdditional& other):
    status_(other.status_){}
    MessageAdditional() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<Server_MsgT::SERVER_STATUS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageAdditional<Server_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<Server_MsgT::SERVER_STATUS>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<Server_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<Server_MsgT::SERVER_STATUS>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.status_,msg.status_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<Server_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<Server_MsgT::SERVER_STATUS>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.status_,msg.status_);
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<Server_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<Server_MsgT::SERVER_STATUS>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.status_,msg.status_);
        }
    };
}