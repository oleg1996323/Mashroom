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

    template<network::Client_MsgT::type T,typename... ARGS>
    void request(std::stop_token stop,const Socket& socket,MessageProcess<Side::CLIENT>& proc,std::shared_ptr<std::condition_variable_any> cv,ARGS&&... args){
        if(proc.send_message<T>(socket,std::forward<ARGS>(args)...)!=ErrorCode::NONE)
            throw std::runtime_error("Error at sending message");
        else {
            auto receive_msg = [&](){
                if(proc.receive_any_message(socket)!=ErrorCode::NONE)
                    throw std::runtime_error("Error at receiving message");
                else cv->notify_one();
            };
            receive_msg();
            while(proc.has_more().load()){
                cv->wait(proc.locker());
                if(stop.stop_requested())
                return;
                receive_msg();
            }
        }
    }

    class Client:public CommonClient<connection::Process<Client>>{
        private:
        friend struct std::hash<network::Client>;
        friend struct std::equal_to<network::Client>;
        mutable std::shared_ptr<std::condition_variable_any> cv_;
        MessageProcess<Side::CLIENT> mprocess_;
        mutable server::Status server_status_ = server::Status::READY;
        
        public:
        Client(const std::string& host, uint16_t port);
        Client(Client&& other) noexcept;
        Client& operator=(Client&& other) noexcept;
        bool operator==(const Client& other) const noexcept;
        ~Client();
        void cancel();
        template<network::Client_MsgT::type T,typename... ARGS>
        ErrorCode request(bool wait,ARGS&&... args){
            if(socket_){
                socket_->set_no_block(false);
                socket_->set_option(Socket::Option<int>(Socket::Option(1,Socket::Options::KeepAlive)));
                socket_->set_option(Socket::Option<timeval>(Socket::Option(timeval{.tv_sec=5,.tv_usec = 0},Socket::Options::TimeOutIn)));
                socket_->set_option(Socket::Option<timeval>(Socket::Option(timeval{.tv_sec=5,.tv_usec = 0},Socket::Options::TimeOutOut)));
            }
            else{
                return ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,"not established",AT_ERROR_ACTION::CONTINUE);
            }
            try{
                process = std::move(Process::make_process(cv_));
                mprocess_.set_locker(process->locker());
                Process::execute_process(process,::request<T,ARGS...>,*socket_,mprocess_,cv_,std::forward<ARGS>(args)...);
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
            if(socket_)
                socket_->set_no_block(false);
            else ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,"connection not established",AT_ERROR_ACTION::CONTINUE);
            try{
                process = std::move(Process::make_process(cv_));
                mprocess_.set_locker(process->locker());
                Process::execute_process(process,::request<T,ARGS...>,*socket_,mprocess_,cv_,std::forward<ARGS>(args)...);
                if(!process->wait(timeout_sec)){
                    process->request_stop(false,0);
                    process.reset();
                    return ErrorPrint::print_error(ErrorCode::TIMEOUT,"request",AT_ERROR_ACTION::CONTINUE);
                }
                else{
                    try{
                        process->throw_if_ready_and_error();
                        return ErrorCode::NONE;
                    }
                    catch(const std::exception& err){
                        return ErrorPrint::print_error(ErrorCode::CONNECTION_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
                    }
                }
            }
            catch(const std::exception& err){
                return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,err.what(),AT_ERROR_ACTION::CONTINUE);
            }
        }
        template<Server_MsgT::type MSG_T>
        const network::Message<MSG_T>& get_result(int16_t timeout_s) const{
            if(process){
                if(process->ready())
                    process->throw_if_ready_and_error();
                if(!process->wait(timeout_s))
                    throw std::runtime_error("Timeout");
                return mprocess_.get_received_message<MSG_T>();
            }
            else throw std::runtime_error("There are not processes");
        }
        template<Server_MsgT::type MSG_T>
        const network::Message<MSG_T>& get_intermediate_result(int16_t timeout_s) const{
            if(process){
                if(mprocess_.has_more().load()){
                    std::unique_lock lock = process->locker();
                    lock.lock();
                    if(cv_->wait_for(lock,std::chrono::seconds(timeout_s))==std::cv_status::no_timeout)
                        return mprocess_.get_received_message<MSG_T>();
                    else throw std::runtime_error("Timeout");
                }
                else return mprocess_.get_received_message<MSG_T>();
            }
            else throw std::runtime_error("There are not processes");
        }
        bool receive_next_message(){
            if(!mprocess_.has_more().load())
                return false;
            else cv_->notify_one();
        }
        server::Status server_status() const;
    };
}