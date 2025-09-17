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
#include "proc/index.h"

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
        ErrorCode err_;
        MessageProcess<Side::SERVER> mprocess_;
        Process(const Process&) = delete;
        Process& operator=(Process&& other) = delete;   //thread_ may be launched and 
                                                        //containing old "other" pointer
        Process& operator=(const Process& other) = delete;
        ErrorCode __execute_heavy_process__(network::Client_MsgT::type msg_t) const; //
        ErrorCode __execute_light_process__(network::Client_MsgT::type msg_t) const; //asyncronously
        static void reply(std::stop_token stop,const Socket& socket,Process<Server>& proc);
        public:
        Process(Socket sock,const ConnectionPool& pool);
        Process(Process&& other);
        ~Process();
        void collapse() const;
        void collapse_at_ready() const;
        void close() const;
        void close_at_done() const;
    };
}