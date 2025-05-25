#pragma once
#include <network/common/def.h>
#include <network/server/connection_pool.h>
#include <network/server/connection_process.h>

#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include <sys/eventfd.h>
#include <network/common/credentials.h>

using namespace std::string_view_literals;
namespace network::server{
    class Server{
    private:
        static void sigchld_handler(int s);
        friend class connection::Process<Server>;
        network::connection::ConnectionPool connection_pool_;
        std::jthread server_thread_;
        std::stop_token stop_token_;
        addrinfo* server_=nullptr;
        eventfd_t server_interruptor;
        Socket server_socket_=-1;
        bool interrupt_transactions_ = false;
        ErrorCode err_;
        Status status_=Status::INACTIVE;
        static void __launch__(Server*);
        Server();
        void __new_connection__(Socket connected_client);
        ErrorCode __connection_process__(Socket connected_client);
        static ErrorCode __set_no_block__(int);
    public:
        Status get_status() const;
        void launch();
        void stop();
        void close_connections(bool wait_for_end_connections = false);
        void shutdown(bool wait_for_end_connections = false);
        ~Server();
        static std::unique_ptr<Server> make_instance(ErrorCode& err);
    };
}