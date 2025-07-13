#pragma once
#include "network/common/message/msgdef.h"
#include <fstream>
#include <sys/mman.h>
#include "network/common/utility.h"
#include "def.h"

namespace network{

    template<>
    struct MessageAdditional<Client_MsgT::TRANSACTION>{
        /// @brief operation hash
        size_t op_hash_ = 0;
        Transaction op_status_ = Transaction::DECLINE;
        MessageAdditional(const MessageAdditional& other):
        op_hash_(other.op_hash_),op_status_(other.op_status_){}
        MessageAdditional(ErrorCode& err,Transaction op_status):op_status_(op_status){}
        MessageAdditional() = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessageAdditional<Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<Client_MsgT::TRANSACTION>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.op_hash_,msg.op_status_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageAdditional<Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<Client_MsgT::TRANSACTION>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Serial_size<MessageAdditional<Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<Client_MsgT::TRANSACTION>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<Client_MsgT::TRANSACTION>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.op_hash_,msg.op_status_);
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<Client_MsgT::TRANSACTION>>{
        using type = MessageAdditional<Client_MsgT::TRANSACTION>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.op_hash_,msg.op_status_);
        }
    };
}