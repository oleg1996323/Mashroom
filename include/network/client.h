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
#include "abstractclient.h"

namespace network{
    class ClientsHandler;
}
namespace network{

    class Client:public AbstractClient<connection::Process<Client>>{
        private:
        friend struct std::hash<network::Client>;
        friend struct std::equal_to<network::Client>;
        friend typename network::ClientsHandler;
        /**
         * @brief don't block the GUI if integrated
         */
        MessageProcess<Side::CLIENT> mprocess_;
        /**
         * @brief for epoll interruption of processes in thread
         */
        mutable ErrorCode err_ = ErrorCode::NONE;
        mutable server::Status server_status_ = server::Status::READY;
        template<typename... ARGS>
        static void request(Process::Function_t<MessageProcess<Side::CLIENT>&,ARGS...> function,const Socket& socket,MessageProcess<Side::CLIENT>& proc,ARGS&&... args){
            if(proc.send_message(socket,std::forward<ARGS>(args)...)!=ErrorCode::NONE)
                throw std::runtime_error("Error at sending message");
            else {
                if(proc.receive_any_message(socket)!=ErrorCode::NONE)
                    throw std::runtime_error("Error at receiving message");
            }
        }
        Client(const std::string& host, uint16_t port);
        ErrorCode connect()
        public:
        Client(Client&& other) noexcept;
        Client& operator=(Client&& other) noexcept;
        bool operator==(const Client& other) const noexcept;
        ~Client();
        void cancel();
        ErrorCode connect(const std::string& host, const std::string& port);
        static std::expected<std::unique_ptr<Client>,ErrorCode> create_and_connect(const std::string& host, uint16_t port){
            Client client(host,port);

        }
        template<network::Client_MsgT::type T,typename... ARGS>
        ErrorCode request(bool wait,ARGS&&... args){
            try{
                process = std::move(Process::add_process(Process::));
                if(wait)
                    process->wait(-1);
                return ErrorCode::NONE;
            }
            catch(const std::exception& err){
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
            }
        }
        template<network::Client_MsgT::type T,typename... ARGS>
        ErrorCode request(int timeout_sec,ARGS&&... args){
            try{
                process = std::move(Process::add_process(Process::));
                if(!process->wait(timeout_sec)){
                    process->request_stop(false,0);
                    process.reset();
                    return ErrorPrint::print_error(ErrorCode::TIMEOUT,"request",AT_ERROR_ACTION::CONTINUE);
                }
                else return ErrorCode::NONE;
            }
            catch(const std::exception& err){
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
            }
        }
        template<MESSAGE_ID<Side::CLIENT>::type MSG_T>
        auto get_result() const{
            return mprocess_.get_received_message<MSG_T>();
        }
        server::Status server_status() const;
    };
}

template<>
struct std::hash<network::Client>{
    size_t operator()(const network::Client& client) const{
        if(client.client_socket_.->ss_family==AF_INET){
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