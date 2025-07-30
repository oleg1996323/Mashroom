#pragma once
#include <network/common/def.h>
#include <network/common/message/msgdef.h>
#include <optional>
#include <program/data.h>
#include "serialization.h"
using namespace std::chrono;
namespace fs = std::filesystem;
namespace network{

template<>
    struct MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>
    {
        struct BaseCapitalizeResult{
            const Data::TYPE data_type;
            const Data::FORMAT data_format;
            std::vector<char> buffer_;
        };

        std::vector<BaseCapitalizeResult> blocks_;
        public:
        MessageAdditional(ErrorCode& err,Data::ACCESS access, const std::vector<std::pair<Data::TYPE,Data::FORMAT>>& to_match){}
        MessageAdditional(const MessageAdditional& other):blocks_(other.blocks_){}
        MessageAdditional() = default;
        bool add_block(Data::TYPE T,Data::FORMAT F, Data::ACCESS A);
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.blocks_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.blocks_);
        }
    };

    template<>
    struct Serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.blocks_);
        }
    };

    template<>
    struct Min_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.blocks_);
        }
    };

    template<>
    struct Max_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.blocks_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.data_format,msg.data_type);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.data_format,msg.data_type);
        }
    };

    template<>
    struct Serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult;
        size_t operator()(const type& msg) noexcept{
            return serial_size(msg.data_format,msg.data_type);
        }
    };

    template<>
    struct Min_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult;
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(msg.data_format,msg.data_type);
        }
    };

    template<>
    struct Max_serial_size<network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>{
        using type = network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(msg.data_format,msg.data_type);
        }
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>);
static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Server_MsgT::DATA_REPLY_CAPITALIZE>::BaseCapitalizeResult>);