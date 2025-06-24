#pragma once
#include <network/common/message/msgdef.h>

namespace network{
    template<>
    struct MessageAdditional<Client_MsgT::SERVER_STATUS>{
        MessageAdditional(const MessageAdditional& other) = default;
        MessageAdditional() = default;
    };
}