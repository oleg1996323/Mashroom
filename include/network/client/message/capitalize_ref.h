#pragma once
#include "network/common/message/msgdef.h"

namespace network{
    template<>
    struct MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>{
        public:
        MessageAdditional(const MessageAdditional& other) = default;
        MessageAdditional(MessageAdditional&& other)=default;
        MessageAdditional() = default;
        MessageAdditional& operator=(const MessageAdditional& other) = default;
        MessageAdditional& operator=(MessageAdditional&& other) noexcept = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>{
        using type = MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>{
        using type = MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>{
        using type = MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>;
        size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>{
        using type = MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>{
        using type = MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };
}

static_assert(serialization::deserialize_concept<true,network::MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>);
static_assert(serialization::deserialize_concept<false,network::MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>);
static_assert(serialization::serialize_concept<true,network::MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>);
static_assert(serialization::serialize_concept<false,network::MessageAdditional<network::Client_MsgT::CAPITALIZE_REF>>);