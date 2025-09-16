#pragma once
#include <network/common/def.h>
#include "network/common/message/message_process.h"
#include "network/common/connection_process.h"
#include "abstractprocess.h"

namespace network{
    class Client;
}

using namespace network;

namespace network::connection{
    template<>
    class Process<Client>:public AbstractProcess<Process<Client>>{
        public:
        Process() = default;
        Process(const Process&) = delete;
        Process(Process&& other) noexcept;
        Process& operator=(const Process&) = delete;
        Process& operator=(Process&& other) noexcept;

        virtual void action_if_process_busy() override{
            throw std::runtime_error("Process doesn't finished!");
        }
    };
}