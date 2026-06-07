#pragma once
#include "web/common/msgdef.h"

namespace network{
template<>
struct Message<network::Server_MsgT::SERVER_STATUS>{
    server::Status status_ = server::Status::READY;
    Message(server::Status status){}
    Message(const Message& other):
    status_(other.status_){}
    Message(Message&& other):
    status_(other.status_){}
    Message& operator=(const Message& other) = default;
    Message& operator=(Message&& other) noexcept = default;
    Message() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::SERVER_STATUS>>{
        using type = Message<network::Server_MsgT::SERVER_STATUS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::SERVER_STATUS>>{
        using type = Message<network::Server_MsgT::SERVER_STATUS>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_);
        }
    };

    template<>
    struct Serial_size<Message<network::Server_MsgT::SERVER_STATUS>>{
        using type = Message<network::Server_MsgT::SERVER_STATUS>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.status_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Server_MsgT::SERVER_STATUS>>{
        using type = Message<network::Server_MsgT::SERVER_STATUS>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::status_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Server_MsgT::SERVER_STATUS>>{
        using type = Message<network::Server_MsgT::SERVER_STATUS>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::status_)>();
        }();
    };
}