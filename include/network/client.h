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

namespace network{
    class ClientsHandler;
}
namespace network{

    class Client{
        private:
        friend struct std::hash<network::Client>;
        friend struct std::equal_to<network::Client>;
        friend typename network::ClientsHandler;
        network::connection::Process<Client> process_;
        friend decltype(process_);
        std::jthread client_thread_;
        std::unique_ptr<sockaddr_storage> client_;
        Socket client_socket_ = -1;
        eventfd_t client_interruptor;
        mutable ErrorCode err_ = ErrorCode::NONE;
        mutable server::Status server_status_ = server::Status::READY;
        bool temporary_ = true;
        bool retry_connection_ = false;
        Client(const std::string& host, const std::string& port);
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
            return process_.send<T>(std::forward<ARGS>(args)...);
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