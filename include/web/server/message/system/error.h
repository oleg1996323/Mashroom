#pragma once
#include "web/common/msgdef.h"
#include "sys/error_code.h"

namespace network{
    template<>
    class Message<network::Server_MsgT::ERROR>{
        server::Status status_;
        ErrorCode err_ = ErrorCode::NONE;
        std::string description_;
        template<bool,auto>
        friend struct serialization::Serialize;
        template<bool,auto>
        friend struct serialization::Deserialize;
        template<auto>
        friend struct serialization::Serial_size;
        template<auto>
        friend struct serialization::Min_serial_size;
        template<auto>
        friend struct serialization::Max_serial_size;
        public:
        Message(ErrorCode error_code,
                    std::string description,
                    server::Status status = server::Status::READY):
                    status_(status),
                    err_(error_code){}
        Message() = default;
        Message(const Message&) = default;
        Message(Message&&) = default;
        Message& operator=(const Message& other) = default;
        Message& operator=(Message&& other) noexcept = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Server_MsgT::ERROR>>{
        using type = Message<network::Server_MsgT::ERROR>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.err_,msg.description_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Server_MsgT::ERROR>>{
        using type = Message<network::Server_MsgT::ERROR>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.status_,msg.err_,msg.description_);
        }
    };

    template<>
    struct Serial_size<Message<network::Server_MsgT::ERROR>>{
        using type = Message<network::Server_MsgT::ERROR>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.status_,msg.err_,msg.description_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Server_MsgT::ERROR>>{
        using type = Message<network::Server_MsgT::ERROR>;
        static constexpr size_t value = []()
        {
            return min_serial_size<
                decltype(type::status_),
                decltype(type::err_),
                decltype(type::description_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Server_MsgT::ERROR>>{
        using type = Message<network::Server_MsgT::ERROR>;
        static constexpr size_t value = []()
        {
            return max_serial_size<
                decltype(type::status_),
                decltype(type::err_),
                decltype(type::description_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Server_MsgT::ERROR>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Server_MsgT::ERROR>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Server_MsgT::ERROR>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Server_MsgT::ERROR>>);