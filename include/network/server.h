#pragma once
#include <network/def.h>
#include <network/connection_pool.h>
#include <network/connection_process.h>

#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>

using namespace std::string_view_literals;
namespace connection{
    class Process;
}
namespace server{
    class Server{
        private:
        static void sigchld_handler(int s);
        friend class connection::Process;
        connection::ConnectionPool connection_pool_;
        std::jthread server_thread_;
        std::stop_token stop_token_;
        addrinfo* server_=nullptr;
        Socket server_socket_;
        bool interrupt_transactions_ = false;
        ErrorCode err_;
        Status status_=Status::INACTIVE;
        static void __launch__(Server*);
        Server();
        void __new_connection__(Socket connected_client);
        ErrorCode __connection_process__(Socket connected_client);
        public:
        Status get_status() const;
        void launch();
        void stop();
        void close_connections(bool wait_for_end_connections = false);
        void shutdown(bool wait_for_end_connections = false);
        static void collapse(Server*,bool);
        static std::ostream& print_ip_port(std::ostream& stream,addrinfo* addr);
        ~Server();
        static std::unique_ptr<Server> make_instance(ErrorCode& err);
    };
}