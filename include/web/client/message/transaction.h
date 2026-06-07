#pragma once
#include "web/common/msgdef.h"
#include <fstream>
#include <sys/mman.h>
#include "network/utility.h"

namespace network{

    template<>
    struct Message<network::Client_MsgT::TRANSACTION>{
        /// @brief operation hash
        size_t op_hash_ = 0;
        Transaction op_status_ = Transaction::DECLINE;
        Message(const Message& other) = delete;
        Message(Message&& other):
        op_hash_(other.op_hash_),op_status_(other.op_status_){}
        Message(Transaction op_status):op_status_(op_status){}
        Message() = default;
        Message& operator=(const Message& other) = delete;
        Message& operator=(Message&& other) noexcept = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.op_hash_,msg.op_status_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Serial_size<Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Min_serial_size<Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::op_hash_),decltype(type::op_status_)>();
        }();
    };

    template<>
    struct Max_serial_size<Message<network::Client_MsgT::TRANSACTION>>{
        using type = Message<network::Client_MsgT::TRANSACTION>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::op_hash_),decltype(type::op_status_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::Message<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::deserialize_concept<false,network::Message<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::serialize_concept<true,network::Message<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::serialize_concept<false,network::Message<network::Client_MsgT::TRANSACTION>>);