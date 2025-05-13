#pragma once
#include <thread>
#include <unistd.h>
#include <network/common/def.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <network/common/credentials.h>

namespace client{
    class Client{
        private:
        std::jthread client_thread_;
        addrinfo* client_=nullptr;
        Socket client_socket_ = -1;
        eventfd_t server_interruptor;
        ErrorCode err_;
        mutable server::Status server_status_;
        Client();
        public:
        ~Client();
        void cancel();
        void disconnect();
        void request();
        void server_status();
        static std::unique_ptr<Client> make_instance(ErrorCode& err);
    };
}