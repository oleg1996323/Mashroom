#pragma once
#include <network/common/def.h>
#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include "network/common/message/message_process.h"
#include <queue>
#include <shared_mutex>
#include "network/common/connection_process.h"
#include "abstractqueuableprocess.h"

namespace network{
    class Server;
}
using namespace network;

namespace network::connection{
    class ConnectionPool;
    template<>
    class Process<Server>:public AbstractQueuableProcess<Process<Server>>{
        friend class ConnectionPool;
        private:
        MessageProcess<Side::SERVER> mprocess_;
        Process(const Process&) = delete;
        Process& operator=(const Process& other) = delete;
        Process(Process&& other) noexcept;
        Process& operator=(Process&& other) noexcept;
        void reply(std::stop_token stop,const Socket& socket);
        public:
        Process():AbstractQueuableProcess(){}
        
        ~Process() = default;
        void collapse(bool wait_finish,uint16_t timeout_sec);
        void close(bool wait_finish,uint16_t timeout_sec);
    };
}