#pragma once
#include <thread>
#include <unistd.h>
#include "network/common/def.h"
#include <sys/eventfd.h>
#include <poll.h>
#include "network/common/credentials.h"
#include "network/common/message/message_process.h"
#include <variant>
#include "network/client/connection_process.h"
#include <netdb.h>
#include <future>

namespace network{
    class ClientsHandler;
}
namespace network{

    class Client{
        private:
        friend struct std::hash<network::Client>;
        friend struct std::equal_to<network::Client>;
        friend typename network::ClientsHandler;
        friend class network::connection::Process<Client>;
        /**
         * @brief don't block the GUI if integrated
         */
        std::unique_ptr<connection::Process<Client>> process;
        Socket client_socket_;
        /**
         * @brief for epoll interruption of processes in thread
         */
        eventfd_t client_interruptor;
        mutable ErrorCode err_ = ErrorCode::NONE;
        mutable server::Status server_status_ = server::Status::READY;
        Client(const std::string& host, uint16_t port);
        public:
        Client(Client&& other) noexcept;
        Client& operator=(Client&& other) noexcept;
        bool operator==(const Client& other) const noexcept;
        ~Client();
        void cancel();
        ErrorCode connect(const std::string& host, const std::string& port);
        bool is_connected() const;
        ErrorCode disconnect();
        template<network::Client_MsgT::type T,typename... ARGS>
        ErrorCode request(ARGS&&... args) const{
            if(client_thread_)
                client_thread_->first.join();
            client_thread_ = std::make_unique<ThreadResult>();
            client_thread_->first = std::move(std::jthread([&socket = this->client_socket_,&promise = this->client_thread_->second,...Args = std::forward<ARGS>(args)](std::stop_token stoken) mutable{
                std::unique_ptr<network::connection::Process<Client>> process_ = 
                    std::make_unique<network::connection::Process<Client>>(socket);
                promise.set_value(process_->send<T>(std::forward<ARGS>(Args)...));
            }));            
            return ErrorCode::NONE;
        }

        server::Status server_status() const;
        // static std::unique_ptr<Client> make_instance(const std::string&,ErrorCode&);
    };
}

template<>
struct std::hash<network::Client>{
    size_t operator()(const network::Client& client) const{
        if(client.client_->ss_family==AF_INET){
            auto addr = (sockaddr_in*)client.client_.get();
            return std::hash<size_t>{}(addr->sin_addr.s_addr)^std::hash<size_t>{}(addr->sin_port);
        }
        else if(client.client_->ss_family==AF_INET6){
            auto addr = (sockaddr_in6*)client.client_.get();
            const uint64_t* addr_seq = (const uint64_t*)(&addr->sin6_addr);
            return std::hash<size_t>{}(addr_seq[0])^ std::hash<size_t>{}(addr_seq[1])^std::hash<size_t>{}(addr->sin6_port);
        }
        else{
            return 0;
        }
    }
};

template<>
struct std::equal_to<network::Client>{
    bool operator()(const network::Client& lhs,const network::Client& rhs) const{
    if (lhs.client_.get() == rhs.client_.get()) return true;  // Один и тот же указатель
    if (!lhs.client_.get() || !rhs.client_.get()) return false;  // Один из них nullptr
    if (lhs.client_->ss_family != rhs.client_->ss_family) {
        return false;
    }
    switch (lhs.client_->ss_family) {
        case AF_INET: {  // IPv4
            const sockaddr_in* a4 = reinterpret_cast<const sockaddr_in*>(lhs.client_.get());
            const sockaddr_in* b4 = reinterpret_cast<const sockaddr_in*>(rhs.client_.get());
            return (a4->sin_port == b4->sin_port) &&
                   (a4->sin_addr.s_addr == b4->sin_addr.s_addr);
        }
        case AF_INET6: {  // IPv6
            const sockaddr_in6* a6 = reinterpret_cast<const sockaddr_in6*>(lhs.client_.get());
            const sockaddr_in6* b6 = reinterpret_cast<const sockaddr_in6*>(rhs.client_.get());
            return (a6->sin6_port == b6->sin6_port) &&
                   (memcmp(&a6->sin6_addr, &b6->sin6_addr, sizeof(in6_addr)) == 0) &&
                   (a6->sin6_scope_id == b6->sin6_scope_id);
        }
        default:
            return memcmp(lhs.client_.get(), rhs.client_.get(), sizeof(sockaddr_storage)) == 0;
    }
    }
};