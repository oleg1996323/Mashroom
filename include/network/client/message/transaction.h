#pragma once
#include "network/common/message/msgdef.h"
#include <fstream>
#include <sys/mman.h>
#include "utility.h"

namespace network{

    template<>
    struct MessageAdditional<network::Client_MsgT::TRANSACTION>{
        /// @brief operation hash
        size_t op_hash_ = 0;
        Transaction op_status_ = Transaction::DECLINE;
        MessageAdditional(const MessageAdditional& other) = delete;
        MessageAdditional(MessageAdditional&& other):
        op_hash_(other.op_hash_),op_status_(other.op_status_){}
        MessageAdditional(Transaction op_status):op_status_(op_status){}
        MessageAdditional() = default;
        MessageAdditional& operator=(const MessageAdditional& other) = delete;
        MessageAdditional& operator=(MessageAdditional&& other) noexcept = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<network::Client_MsgT::TRANSACTION>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.op_hash_,msg.op_status_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<network::Client_MsgT::TRANSACTION>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<network::Client_MsgT::TRANSACTION>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<network::Client_MsgT::TRANSACTION>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::op_hash_),decltype(type::op_status_)>();
        }();
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<network::Client_MsgT::TRANSACTION>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::op_hash_),decltype(type::op_status_)>();
        }();
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Client_MsgT::TRANSACTION>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Client_MsgT::TRANSACTION>>);
static_assert(std::is_move_constructible_v<network::MessageAdditional<network::Client_MsgT::TRANSACTION>>);
static_assert(std::is_move_assignable_v<network::MessageAdditional<network::Client_MsgT::TRANSACTION>>);