#pragma once
#include "web/common/msgdef.h"
#include "web/common/detail/progress_def.h"
#include "web/common/detail/transaction.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#ifdef DEBUG
#include <gtest/gtest.h>
#endif

namespace network{
    template<>
    class Message<network::Client_MsgT::PROGRESS>:
            public Message<Client_MsgT::TRANSACTION>{
        private:
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
                return Message<Client_MsgT::TRANSACTION>::operator==(other);
            }
        #endif
        public:
        Message() = default;
        Message(Message<Client_MsgT::TRANSACTION>
            transaction) 
            noexcept:
            Message<Client_MsgT::TRANSACTION>(std::move(transaction))
        {}
        Message(const Message& other):
        Message<Client_MsgT::TRANSACTION>(other){}
        Message(Message&& other) noexcept:
        Message<Client_MsgT::TRANSACTION>(std::move(other)){}
        Message& operator=(const Message& other) noexcept{
            Message<Client_MsgT::TRANSACTION>::operator=(other);
            return *this;
        }
        Message& operator=(Message&& other) noexcept{
            Message<Client_MsgT::TRANSACTION>::operator=(std::move(other));
            return *this;
        }
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::PROGRESS>>{
        using type = Message<network::Client_MsgT::PROGRESS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(
                msg,
                buf,
                static_cast<const Message<Client_MsgT::TRANSACTION>&>(msg));
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::PROGRESS>>{
        using type = Message<network::Client_MsgT::PROGRESS>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(
                msg,
                buf,
                static_cast<Message<Client_MsgT::TRANSACTION>&>(msg));
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::PROGRESS>>{
        using type = Message<network::Client_MsgT::PROGRESS>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(
                static_cast<const Message<Client_MsgT::TRANSACTION>&>(msg));
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::PROGRESS>>{
        using type = Message<network::Client_MsgT::PROGRESS>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<Message<Client_MsgT::TRANSACTION>>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::PROGRESS>>{
        using type = Message<network::Client_MsgT::PROGRESS>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<Message<Client_MsgT::TRANSACTION>>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::PROGRESS>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::PROGRESS>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::PROGRESS>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::PROGRESS>>);
static_assert(std::is_move_constructible_v<network::Message<network::Client_MsgT::PROGRESS>>);