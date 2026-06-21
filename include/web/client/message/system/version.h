#pragma once
#include "serialization.h"
#include "web/common/msgdef.h"
#ifdef DEBUG
#include <gtest/gtest.h>
#endif

namespace network{
    template<>
    class Message<network::Client_MsgT::VERSION>{
        uint64_t version_;
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
        #ifdef DEBUG
            FRIEND_TEST(NetworkMesssageHandler,MessageHandlerSerializationTest);
            bool operator==(const Message& other) const noexcept{
                return version_==other.version_;
            }
        #endif
        public:
        Message() = default;
        Message(uint64_t version):
        version_(version){}
        Message(const Message& other):
        version_(other.version_){}
        Message(Message&& other):
        version_(other.version_){}
        Message& operator=(const Message& other) noexcept{
            version_=other.version_;
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            version_=other.version_;
            return *this;
        }
        uint64_t version() const noexcept{
            return version_;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::VERSION>>{
        using type = Message<network::Client_MsgT::VERSION>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.version_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::VERSION>>{
        using type = Message<network::Client_MsgT::VERSION>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.version_);
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::VERSION>>{
        using type = Message<network::Client_MsgT::VERSION>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.version_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::VERSION>>{
        using type = Message<network::Client_MsgT::VERSION>;
        static constexpr size_t value = []()
        {
            return min_serial_size<decltype(type::version_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::VERSION>>{
        using type = Message<network::Client_MsgT::VERSION>;
        static constexpr size_t value = []()
        {
            return max_serial_size<decltype(type::version_)>();
        }();
    };
}

static_assert(std::is_move_constructible_v<network::Message<network::Client_MsgT::VERSION>>);