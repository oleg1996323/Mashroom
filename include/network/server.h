#pragma once
#include <network/def.h>
#include <network/connection_process.h>
#include <network/settings/server.h>

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
    using ProcessPool = ::std::list<Process>;
    using ProcessInstance = ::std::list<Process>::iterator;
}

namespace server{
    class Server{
        private:
        static void sigchld_handler(int s);
        friend constexpr std::unique_ptr<Server> std::make_unique<Server>(const Server&);
        friend constexpr std::unique_ptr<Server> std::make_unique<Server>(Server&&);
        friend constexpr std::unique_ptr<Server> std::make_unique<Server>(std::string_view&);
        friend class connection::Process;
        std::unique_ptr<std::thread> server_thread_;
        connection::ProcessPool connection_pool_;
        addrinfo* server_=nullptr;
        Socket server_socket_;
        bool interput_transactions_ = false;
        ErrorCode err_;
        Status status_=Status::INACTIVE;
        static void __launch__(Server* server);
        Server(std::string_view hname);
        void __new_connection__(Socket connected_client);
        public:
        Status get_status() const;
        void launch();
        void close(bool abort_connections = false);
        void shutdown(bool wait_for_end_conenctions = true);
        ~Server();
        static ErrorCode init();
    };
}