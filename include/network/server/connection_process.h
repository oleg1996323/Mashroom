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
        Process& operator=(Process&& other) = delete;   //thread_ may be launched and 
                                                        //containing old "other" pointer
        Process& operator=(const Process& other) = delete;
        static void reply(std::stop_token stop,const Socket& socket,Process<Server>& proc);
        public:
        Process();
        Process(Process&& other);
        ~Process();
        void collapse(bool wait_finish,uint16_t timeout_sec);
        void close(bool wait_finish,uint16_t timeout_sec);
    };
}