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
#include "commonserver.h"

using namespace std::string_view_literals;
namespace network{
    class Server:public CommonServer<connection::ConnectionPool>{
    private:
        std::jthread server_thread_;
        std::stop_token stop_token_;
        server::Status status_=server::Status::INACTIVE;
        Server();
        virtual void after_accept(Socket& socket) override{
            try{
                socket.set_no_block(true).set_option(Socket::Options::KeepAlive,true);
                using Event_t = Multiplexor::Event;
                modify_connection(socket,Event_t::EdgeTrigger);
                std::cout<<"Connecting ";
                socket.print_address_info(std::cout);
            }
            catch(const std::runtime_error& err){
                std::cout<<"Error after accepting connection"<<std::endl;
                return;
            }
            return;
        }
    public:
        server::Status get_status() const;
        void stop();
        void collapse(bool wait_for_end_connections = false);
        ~Server();
        static std::unique_ptr<Server> make_instance(ErrorCode& err);
    };
}