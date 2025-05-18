#pragma once
#include <thread>
#include <unistd.h>
#include <network/common/def.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <network/common/credentials.h>
#include <network/client/message.h>
#include <variant>

namespace client{
    enum class TYPE_MESSAGE:uint8_t;

    class Client{
        private:
        std::string client_name_;
        std::jthread client_thread_;
        addrinfo* servinfo_=nullptr;
        Socket client_socket_ = -1;
        eventfd_t client_interruptor;
        ErrorCode err_;
        mutable server::Status server_status_ = server::Status::READY;
        Client(const std::string& client_name);
        public:
        ~Client();
        void cancel();
        ErrorCode connect();
        bool is_connected() const;
        ErrorCode disconnect();
        template<client::TYPE_MESSAGE T>
        ErrorCode request(client::Message<T>);

        server::Status server_status() const;
        static std::unique_ptr<Client> make_instance(const std::string&,ErrorCode&);
    };
}