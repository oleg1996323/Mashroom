#pragma once
#include <network/common/def.h>
#include <network/server/message.h>
#include <network/client/message.h>
#include <network/common/connection_process.h>
#include <network/client/request_scheduler.h>

namespace network::client{
    class Client;
}

using namespace network::client;

namespace network::connection{
    template<>
    class Process<Client>{
        public:
        
        private:
        friend class network::client::Client;
        Process() = default;
        Process(const Process&) = delete;
        Process& operator=(const Process&) = delete;
        Process(Process&& other) noexcept;
        void swap(Process& other) noexcept;
        Process& operator=(Process&& other) noexcept;
        Scheduler scheduler_;
    };
}