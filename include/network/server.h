#pragma once
#include "network/common/def.h"
#include "network/server/connection_pool.h"
#include "network/server/connection_process.h"

#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include <sys/eventfd.h>
#include "network/common/credentials.h"
#include "network/common/abstractserver.h"

using namespace std::string_view_literals;
namespace network{
    class Server:public AbstractServer{
    private:
        friend class connection::Process<Server>;
        network::connection::ConnectionPool connection_pool_;
        std::jthread server_thread_;
        std::stop_token stop_token_;
        std::unique_ptr<sockaddr_storage> server_;
        eventfd_t server_interruptor;
        Socket server_socket_;
        bool interrupt_transactions_ = false;
        ErrorCode err_;
        server::Status status_=server::Status::INACTIVE;
        static void __launch__(Server*);
        Server();
        virtual ServerError before_accept(){
            connection_pool_.process_connection(events[i].data.fd);
        }
        virtual ServerError after_accept(Socket& socket) override{
            try{
                socket.set_no_block(true).set_option(Socket::Options::KeepAlive,true);
                __new_connection__();
                std::cout<<"Connecting ";
                socket.print_address_info(std::cout);
            }
            catch(const std::runtime_error& err){
                return ServerError::ACCEPTER;
            }
            return ServerError::NONE;
        }
        void __new_connection__(const Socket& connected_client);
        ErrorCode __connection_process__(const Socket& connected_client);
    public:
        server::Status get_status() const;
        void launch();
        void stop();
        void close_connections(bool wait_for_end_connections = false);
        void shutdown(bool wait_for_end_connections = false);
        ~Server();
        static std::unique_ptr<Server> make_instance(ErrorCode& err);
    };
}