#pragma once
#include <network/common/def.h>
#include "network/common/message/message_process.h"
#include "network/common/connection_process.h"
#include "network/client/request_scheduler.h"

namespace network{
    class Client;
}

using namespace network;

namespace network::connection{
    template<>
    class Process<Client>{
        public:
        
        private:
        friend class network::Client;
        Process() = default;
        Process(const Process&) = delete;
        Process& operator=(const Process&) = delete;
        Process(Process&& other) noexcept;
        void swap(Process& other) noexcept;
        Process& operator=(Process&& other) noexcept;
        Scheduler scheduler_;
    };
}