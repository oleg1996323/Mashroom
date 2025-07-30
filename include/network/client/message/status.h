#pragma once
#include <network/common/message/msgdef.h>

namespace network{
    template<>
    struct MessageAdditional<network::Client_MsgT::SERVER_STATUS>{
        MessageAdditional(const MessageAdditional& other) = default;
        MessageAdditional() = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<network::Client_MsgT::SERVER_STATUS>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<network::Client_MsgT::SERVER_STATUS>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Client_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<network::Client_MsgT::SERVER_STATUS>;
        size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Client_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<network::Client_MsgT::SERVER_STATUS>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Client_MsgT::SERVER_STATUS>>{
        using type = MessageAdditional<network::Client_MsgT::SERVER_STATUS>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Client_MsgT::SERVER_STATUS>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Client_MsgT::SERVER_STATUS>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Client_MsgT::SERVER_STATUS>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Client_MsgT::SERVER_STATUS>>);