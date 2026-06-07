#pragma once
#include "web/common/msgdef.h"
#include "web/common/message_handler.h"
#include "web/common/connection_process.h"
#include "network/abstractprocess.h"

namespace network{
    class ClientConnectionProcess:public AbstractRequestableConnectionProcess{
        MessageHandler<Side::CLIENT> send_hmsg_;
        MessageHandler<Side::SERVER> recv_hmsg_;
    };
}