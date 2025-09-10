#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <thread>
#include "network/common/def.h"
#include "sys/config/server.h"
#include "network/server/connection_pool.h"

namespace network{
    enum ErrorCode{
        NONE
    };
    class AbstractServer{
        std::list<Socket> peers_;
        ConnectionPool pool_; //verify
        Socket socket_ = -1;
        public:
        AbstractServer(const server::Settings& settings);
        virtual ErrorCode before_init() = 0;
        virtual ErrorCode init() = 0;
        virtual ErrorCode after_init() = 0;
        virtual ErrorCode before_connect() = 0;
        ErrorCode connect();
        virtual ErrorCode after_connect() = 0;
        virtual ErrorCode before_listen() = 0;
        ErrorCode listen();
        virtual ErrorCode after_listen(Socket accepted) = 0;
        virtual ErrorCode before_accept(Socket accepted) = 0;
        virtual ErrorCode after_accept(Socket accepted) = 0;
        virtual ErrorCode at_exit() = 0;
        const std::list<Socket>& peers_list() const;
        const Socket socket() const;
        void close();
        void collapse();
        ErrorCode launch();
        //fork
        //thread
    };
}