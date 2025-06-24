#pragma once
#include "network/common/message/msgdef.h"

namespace network{
    template<>
    struct MessageAdditional<Client_MsgT::CAPITALIZE_REF>{
        public:
        MessageAdditional(const MessageAdditional& other) = default;
        MessageAdditional(){}
    };
}