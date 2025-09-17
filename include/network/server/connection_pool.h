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
    class ConnectionPool:public AbstractConnectionPool<Process<Server>>{
        public:
        ConnectionPool(const Server& server);
        ~ConnectionPool();
        //make it by UDP
        void notify_status_all() const;
        void shutdown_all();
        void shut_not_processing();
        void close_all_at_not_busy();
        void close_all();
        network::server::Status server_status() const;
    };
}
