#pragma once
#include "network/common/message/msgdef.h"

namespace network{
    template<>
    class MessageAdditional<Client_MsgT::CAPITALIZE_REF>{
        MessageAdditional(){}
    };
}