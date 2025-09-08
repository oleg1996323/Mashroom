#pragma once
#include <unordered_set>
#include <thread>
#include <mutex>
#include <network/server/connection_process.h>
#include <shared_mutex>

namespace network::connection{
    class ConnectionPool{
        ::std::unordered_set<network::connection::Process<Server>> data_;
        std::mutex locker_;
        std::jthread thread_;
        const Server& server_;
        std::condition_variable notiifier_cv_;
        mutable std::atomic<bool> is_running_;
        using iterator = decltype(data_)::iterator;
        static void __proc_notifier__(ConnectionPool&);
        public:
        ConnectionPool(const Server& server);
        ~ConnectionPool();
        void notify_status_all() const;
        void shutdown_all();
        void shut_not_processing();
        void close_all_at_not_busy();
        void close_all();
        ErrorCode process_connection(Socket sock) const;
        network::server::Status server_status() const;
        bool is_connected(Socket socket) const;
        ConnectionPool::iterator add_connection(Socket connection_socket, size_t buffer_sz=4096);
        void update_connections();
    };
}