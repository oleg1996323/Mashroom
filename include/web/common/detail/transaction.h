#pragma once
#include "web/common/msgdef.h"
#include "web/client/message/system/transaction.h"
#include "web/server/message/system/transaction.h"

namespace network{
    Message<Server_MsgT::TRANSACTION> 
            get_reply(const Message<Client_MsgT::TRANSACTION>&) noexcept;
    Message<Client_MsgT::TRANSACTION> 
            get_reply(const Message<Server_MsgT::TRANSACTION>&) noexcept;
}