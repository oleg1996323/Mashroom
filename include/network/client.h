#pragma once
#include <thread>
#include <unistd.h>
#include "network/common/def.h"
#include <sys/eventfd.h>
#include <poll.h>
#include "network/common/credentials.h"
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
    void request(std::error_code& err,
            Socket socket,
            std::stop_token st,
            connection::Process<Client>* proc,
            ARGS&&... args){
        if(!proc){
            err = std::make_error_code(std::errc::invalid_argument);
            return;
        }
        proc->send_message<T>(err,socket,std::forward<ARGS>(args)...);
        if(err!=std::error_code())
            return;
        else {
            proc->receive_message(socket,err);
            if(err!=std::error_code())
                return;
            if(proc->has_more().load()){
                std::vector<std::vector<char>> buffers_;
                while(proc->has_more().load()){
                    if(st.stop_requested())
                        return;
                    proc->receive_message(socket,err);
                    if(err!=std::error_code())
                        return;
                    else{
                        if(auto msg = proc->get_received_msg().lock();
                            msg)
                            buffers_.push_back(msg->buffer());
                        else continue;
                    }
                }
            }
        }
    }

    class Client:public CommonClient{
        private:
        friend struct std::hash<network::Client>;
        friend struct std::equal_to<network::Client>;
        mutable std::condition_variable cv_;
        mutable server::Status server_status_ = server::Status::READY;
        public:
        Client(const std::string& host, uint16_t port);
        Client(Client&& other) = delete;
        Client& operator=(Client&& other) = delete;
        bool operator==(const Client& other) const noexcept;
        ~Client();
        void cancel();
        using Process = connection::Process<network::Client>;
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
                std::error_code err;
                auto process = std::make_unique<Process>(cv_);
                process->emplace_task(
                    ::request<T,ARGS...>,
                    err,
                    process->get_stop_source().get_token(),
                    socket_,
                    process_.get(),
                    std::forward<ARGS>(args)...);
                if(err!=std::error_code())
                    return ErrorCode::INTERNAL_ERROR;
                else
                    add_request(std::move(process),err);
                if(wait)
                    process_->wait(-1,err);
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
                std::error_code err;
                auto process = std::make_unique<Process>(cv_);
                process->emplace_task(
                    ::request<T,ARGS...>,
                    err,
                    process->get_stop_source().get_token(),
                    *socket_,
                    process.get(),
                    std::forward<ARGS>(args)...);
                if(err!=std::error_code())
                    return ErrorCode::INTERNAL_ERROR;
                else
                    add_request(std::move(process),err);
                if(!process_->wait(timeout_sec,err)){
                    process_->request_stop(false,0,err);
                    process_.reset();
                    return ErrorPrint::print_error(ErrorCode::TIMEOUT,"request",AT_ERROR_ACTION::CONTINUE);
                }
                else{
                    try{
                        auto result = process_->get_result<Message<T>>(timeout_sec,err);
                        if(!result.has_value())
                            return ErrorCode::RECEIVING_MESSAGE_ERROR;
                        else return ErrorCode::NONE;
                    }
                    catch(const std::exception& err){
                        return ErrorPrint::print_error(
                            ErrorCode::CONNECTION_ERROR,
                            err.what(),
                            AT_ERROR_ACTION::CONTINUE);
                    }
                }
            }
            catch(const std::exception& err){
                return ErrorPrint::print_error(
                            ErrorCode::INTERNAL_ERROR,
                            err.what(),
                            AT_ERROR_ACTION::CONTINUE);
            }
        }

        const std::weak_ptr<const network::MessageHandler<network::Side::SERVER>> get_result(
            int16_t timeout_s,
            std::error_code& err) const{
            if(process_){
                if(auto proc_ptr = dynamic_cast<const 
                    connection::Process<Client>*>(
                        process_.get());
                        proc_ptr==nullptr)
                {
                    throw std::runtime_error("There are not processes");
                }
                else{
                    auto lk = proc_ptr->locker();
                    if(!cv_.wait_for(
                        lk,
                        std::chrono::seconds(timeout_s),
                        [this,proc_ptr,&err]()->bool
                        {
                            return process_->is_ready(err) &&
                                proc_ptr->get_received_msg().lock().get()!=
                                nullptr;
                        }))
                    {
                        err = std::make_error_code(std::errc::timed_out);
                        return std::weak_ptr<network::MessageHandler<
                                    network::Side::SERVER>>();
                    }
                    else return proc_ptr->get_received_msg();
                }
            }
            else {
                err = std::make_error_code(std::errc::timed_out);
                return std::weak_ptr<network::MessageHandler<
                            network::Side::SERVER>>();
            }
        }
        const std::weak_ptr<const network::MessageHandler<network::Side::SERVER>> get_intermediate_result
            (int16_t timeout_s,std::error_code& err) const{
            if(process_){
                if(auto proc_ptr = dynamic_cast<const 
                    connection::Process<Client>*>(
                        process_.get());
                        proc_ptr==nullptr)
                {
                    throw std::runtime_error("There are not processes");
                }
                else{
                    auto lk = proc_ptr->locker();
                    if(!cv_.wait_for(
                        lk,
                        std::chrono::seconds(timeout_s),
                        [this,proc_ptr,&err]()->bool
                        {
                            return proc_ptr->get_received_msg().lock().get()!=
                                nullptr;
                        }))
                    {
                        err = std::make_error_code(std::errc::timed_out);
                        return std::shared_ptr<network::MessageHandler<
                                    network::Side::SERVER>>();
                    }
                    else return proc_ptr->get_received_msg();
                }
            }
            else {
                err = std::make_error_code(std::errc::timed_out);
                return std::shared_ptr<network::MessageHandler<
                            network::Side::SERVER>>();
            }
        }
        server::Status server_status() const;
    };
}