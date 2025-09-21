#pragma once
#include <unordered_set>
#include <thread>
#include <mutex>
#include <network/server/connection_process.h>
#include <shared_mutex>
#include "multiplexor.h"
#include "commonsocket.h"
#include "abstractconnectionpool.h"

namespace network::connection{
    class ConnectionPool:public AbstractConnectionPool<Process<network::Server>>{
        const network::Server& server_;
        public:
        ConnectionPool(const Server& server):AbstractConnectionPool(),server_(server){}
        ~ConnectionPool();
        //make it by UDP
        virtual void execute(std::stop_token stop,const Socket& socket) override{
            const std::unique_ptr<Process<Server>>& process = this->process(socket);
            Process<Server>::reply(stop,socket,*process);
        }
        network::server::Status server_status() const;
    };
}
