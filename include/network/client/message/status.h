#pragma once
#include <network/common/message/msgdef.h>

namespace network{
    template<>
    struct Message<Client_MsgT::SERVER_STATUS>{};
}