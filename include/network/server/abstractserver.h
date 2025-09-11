#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <thread>
#include "network/common/def.h"
#include "sys/config/server.h"
#include "network/server/connection_pool.h"
#include "concepts.h"
#include "network/common/socket.h"

namespace network{
    enum ServerError{
        NONE
    };
    class AbstractServer{
        std::list<Socket> peers_;
        std::unique_ptr<sockaddr_storage> socket_in_;
        Socket socket_ = -1;
        ServerError err_;
        public:
        AbstractServer(const server::Settings& settings);
        virtual ServerError before_init() = 0;
        virtual ServerError init(const std::vector<std::string>& args){};
        virtual ServerError after_init() = 0;
        virtual ServerError before_connect() = 0;
        ServerError connect();
        virtual ServerError after_connect() = 0;
        virtual ServerError before_listen() = 0;
        ServerError listen();
        virtual ServerError after_listen(Socket accepted) = 0;
        virtual ServerError before_accept(Socket accepted) = 0;
        virtual ServerError after_accept(Socket accepted) = 0;
        virtual ServerError at_close() = 0;
        const std::list<Socket>& peers_list() const;
        const Socket socket() const;
        void close();
        void collapse();
        ServerError launch();
        //fork
        //thread
    };
}