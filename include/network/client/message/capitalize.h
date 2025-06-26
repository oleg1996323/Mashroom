#pragma once
#include <network/common/message/msgdef.h>

namespace network{
    template<>
    struct MessageAdditional<Client_MsgT::CAPITALIZE>{
        MessageAdditional(const MessageAdditional& other) = default;
        MessageAdditional() = default;
    };
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,MessageAdditional<Client_MsgT::CAPITALIZE>>{
        using type = MessageAdditional<Client_MsgT::CAPITALIZE>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,MessageAdditional<Client_MsgT::CAPITALIZE>>{
        using type = MessageAdditional<Client_MsgT::CAPITALIZE>;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            return SerializationEC::NONE;
        }
    };

    template<>
    struct Serial_size<MessageAdditional<Client_MsgT::CAPITALIZE>>{
        using type = MessageAdditional<Client_MsgT::CAPITALIZE>;
        size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Min_serial_size<MessageAdditional<Client_MsgT::CAPITALIZE>>{
        using type = MessageAdditional<Client_MsgT::CAPITALIZE>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };

    template<>
    struct Max_serial_size<MessageAdditional<Client_MsgT::CAPITALIZE>>{
        using type = MessageAdditional<Client_MsgT::CAPITALIZE>;
        constexpr size_t operator()(const type& msg) noexcept{
            return 0;
        }
    };
}