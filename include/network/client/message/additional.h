#pragma once
#include "network/client/message/index.h"
#include "network/client/message/index_ref.h"
#include "network/client/message/status.h"
#include "network/client/message/data_request.h"
#include "network/client/message/transaction.h"

namespace network{
    template<auto MSG_T,typename... ARGS>
    requires MessageEnumConcept<MSG_T>
    MessageAdditional<MSG_T> make_additional(ARGS&&... args){
        return MessageAdditional<MSG_T>(std::forward<ARGS>(args)...);
    }
}