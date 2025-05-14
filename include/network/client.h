#pragma once
#include <thread>
#include <unistd.h>
#include <network/common/def.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <network/common/credentials.h>
#include <network/common/message.h>

namespace client{
    class Client{
        private:
        std::jthread client_thread_;
        addrinfo* client_=nullptr;
        Socket client_socket_ = -1;
        eventfd_t server_interruptor;
        ErrorCode err_;
        mutable server::Status server_status_;
        Client(const std::string& client_name);
        public:
        ~Client();
        void cancel();
        ErrorCode connect();
        ErrorCode disconnect();
        template<network::TYPE_MESSAGE T>
        ErrorCode request(network::Message<T>);
        server::Status server_status() const;
        static std::unique_ptr<Client> make_instance(ErrorCode& err);
    };
}