#pragma once
#include <network/common/message/msgdef.h>

namespace network{
template<>
struct MessageAdditional<network::Server_MsgT::VERSION>{
    server::Status status_;
    uint64_t version_;
    MessageAdditional(ErrorCode& err,uint64_t version, server::Status status):
    status_(status),version_(version){}
    MessageAdditional(const MessageAdditional& other):
    status_(other.status_),
    version_(other.version_){}
    MessageAdditional(MessageAdditional&& other):
    status_(other.status_),
    version_(other.version_){}
    MessageAdditional& operator=(const MessageAdditional& other) = default;
    MessageAdditional& operator=(MessageAdditional&& other) noexcept = default;
    MessageAdditional() = default;
};
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::VERSION>>{
        using type = MessageAdditional<network::Server_MsgT::VERSION>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.version_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::VERSION>>{
        using type = MessageAdditional<network::Server_MsgT::VERSION>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.version_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Server_MsgT::VERSION>>{
        using type = MessageAdditional<network::Server_MsgT::VERSION>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.status_,msg.version_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Server_MsgT::VERSION>>{
        using type = MessageAdditional<network::Server_MsgT::VERSION>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::status_),decltype(type::version_)>();
        }();
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Server_MsgT::VERSION>>{
        using type = MessageAdditional<network::Server_MsgT::VERSION>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::status_),decltype(type::version_)>();
        }();
    };
}