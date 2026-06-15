#include "web/common/detail/transaction.h"

namespace network{
    Message<Server_MsgT::TRANSACTION> 
            get_reply(const Message<Client_MsgT::TRANSACTION>& other) noexcept
    {
        Message<Server_MsgT::TRANSACTION> result;
        result.op_hash_ = other.hash();
        return result;
    }
    Message<Client_MsgT::TRANSACTION> 
            get_reply(const Message<Server_MsgT::TRANSACTION>& other) noexcept
    {
        Message<Client_MsgT::TRANSACTION> result;
        result.op_hash_ = other.hash();
        return result;
    }
}