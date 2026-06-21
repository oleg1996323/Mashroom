#pragma once
#include "web/common/msgdef.h"
#include "serialization.h"
#ifdef DEBUG
#include <gtest/gtest.h>
#endif

namespace network{
    template<>
    class Message<network::Client_MsgT::SERVER_STATUS>{
        public:
        #ifdef DEBUG
            FRIEND_TEST(NetworkMesssageHandler,MessageHandlerSerializationTest);
            bool operator==(const Message& other) const noexcept{
                return true;
            }
        #endif
        public:
        Message(const Message& other) noexcept = default;
        Message(Message&& other) noexcept =default;
        Message() = default;
        Message& operator=(const Message& other) noexcept = default;
        Message& operator=(Message&& other) noexcept = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::SERVER_STATUS>>{
        using type = Message<network::Client_MsgT::SERVER_STATUS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::SERVER_STATUS>>{
        using type = Message<network::Client_MsgT::SERVER_STATUS>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::SERVER_STATUS>>{
        using type = Message<network::Client_MsgT::SERVER_STATUS>;
        size_t operator()(const type& msg) const noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::SERVER_STATUS>>{
        using type = Message<network::Client_MsgT::SERVER_STATUS>;
        static constexpr size_t value = []() ->size_t
        {
            return 0;
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::SERVER_STATUS>>{
        using type = Message<network::Client_MsgT::SERVER_STATUS>;
        static constexpr size_t value = []() ->size_t
        {
            return 0;
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::SERVER_STATUS>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::SERVER_STATUS>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::SERVER_STATUS>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::SERVER_STATUS>>);
static_assert(std::is_move_constructible_v<network::Message<network::Client_MsgT::SERVER_STATUS>>);