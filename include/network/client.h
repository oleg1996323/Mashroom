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
#include "commonclient.h"

namespace network{
    class ClientsHandler;
}
namespace network{

    class Client:public CommonClient<connection::Process<Client>>{
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
        static void request(const Socket& socket,MessageProcess<Side::CLIENT>& proc,ARGS&&... args){
            if(proc.send_message(socket,std::forward<ARGS>(args)...)!=ErrorCode::NONE)
                throw std::runtime_error("Error at sending message");
            else {
                if(proc.receive_any_message(socket)!=ErrorCode::NONE)
                    throw std::runtime_error("Error at receiving message");
            }
        }
        Client(const std::string& host, uint16_t port);
        public:
        Client(Client&& other) noexcept;
        Client& operator=(Client&& other) noexcept;
        bool operator==(const Client& other) const noexcept;
        ~Client();
        void cancel();
        template<network::Client_MsgT::type T,typename... ARGS>
        ErrorCode request(bool wait,ARGS&&... args){
            try{
                process = std::move(Process::add_process(&Client::request,*socket_,mprocess_,std::forward<ARGS>(args)...));
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
                process = std::move(Process::add_process(&Client::request,*socket_,mprocess_,std::forward<ARGS>(args)...));
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