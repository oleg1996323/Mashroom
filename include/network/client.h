#pragma once
#include <thread>
#include <unistd.h>
#include <network/def.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <network/credentials.h>

namespace client{
    class Client{
        private:
        std::jthread client_thread_;
        addrinfo* client_=nullptr;
        Socket client_socket_ = -1;
        eventfd_t server_interruptor;
        public:
        Client();
        ~Client();
        void cancel();
        void disconnect();
        void request();
        void server_status();
        
    };
}