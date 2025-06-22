#pragma once
#include "network/common/message/msgdef.h"
#include <fstream>
#include <sys/mman.h>
#include "network/common/utility.h"
#include "functional/def.h"

namespace network{

    template<>
    struct MessageAdditional<Client_MsgT::TRANSACTION>{
        /// @brief operation hash
        size_t op_hash_ = 0;
        Transaction op_status_ = Transaction::DECLINE;
        MessageAdditional(size_t op_hash,Transaction op_status);
    };
}