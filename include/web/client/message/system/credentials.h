#pragma once
#include "web/common/msgdef.h"

namespace network{
    template<>
    class Message<network::Client_MsgT::CREDENTIALS>{
        std::string login_;
        std::string password_;
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
        Message(std::string login,
                std::string password):
                    login_(login),
                    password_(password){}
        Message() = default;
        Message(const Message&) = default;
        Message(Message&&) = default;
        Message& operator=(const Message& other) = default;
        Message& operator=(Message&& other) noexcept = default;
        const std::string& login() const noexcept{
            return login_;
        }
        const std::string& password() const noexcept{
            return password_;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::CREDENTIALS>>{
        using type = Message<network::Client_MsgT::CREDENTIALS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.login_,msg.password_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::CREDENTIALS>>{
        using type = Message<network::Client_MsgT::CREDENTIALS>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.login_,msg.password_);
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::CREDENTIALS>>{
        using type = Message<network::Client_MsgT::CREDENTIALS>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.login_,msg.password_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::CREDENTIALS>>{
        using type = Message<network::Client_MsgT::CREDENTIALS>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::login_),
                        decltype(type::password_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::CREDENTIALS>>{
        using type = Message<network::Client_MsgT::CREDENTIALS>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::login_),
                        decltype(type::password_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::CREDENTIALS>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::CREDENTIALS>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::CREDENTIALS>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::CREDENTIALS>>);