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
    protected:
        std::jthread server_thread_;
        std::stop_token stop_token_;
        server::Status status_=server::Status::INACTIVE;
        Server(const server::Settings& settings);
        virtual void after_accept(Socket& socket) override;
        virtual void after_listen() override;
        virtual void at_closing() override{
            status_ = server::Status::INACTIVE;
        }
        virtual void at_collapsing() override{
            status_ = server::Status::SUSPENDED;
        }
    public:
        server::Status get_status() const;
        ~Server();
        static std::unique_ptr<Server> make_instance(const server::Settings& settings);
    };
}