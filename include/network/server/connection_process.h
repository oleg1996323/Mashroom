#pragma once
#include <network/common/def.h>
#include <string_view>
#include <cinttypes>
#include <memory>
#include <fstream>
#include <list>
#include <thread>
#include <future>
#include <vector>
#include "network/common/message/message_handler.h"
#include <queue>
#include <shared_mutex>
#include "network/common/connection_process.h"
#include "network/commonsocket.h"

namespace network::connection{
    void send_error(const Socket& socket,
            network::connection::Process<Server>* process,
            ErrorCode error_state,
            server::Status status,
            std::error_code& err){
        using namespace network;
        network::Message<Server_MsgT::ERROR> rep_msg;
        rep_msg.additional().err_ = error_state;
        rep_msg.additional().status_=network::server::Status::READY;
        process->send_message<Server_MsgT::ERROR>(std::stop_token(),socket,err,std::move(rep_msg));
        return;
    }

    template<>
    class Process<Server>:public AbstractConnectionProcess{
        MessageHandler<Side::SERVER> recv_hmsg_;
        public:
        virtual void on_read(std::error_code& err) noexcept override{
            using namespace serialization;
            auto at_error = [this](std::errc c){
                recv_hmsg_ = std::move(recv_hmsg_);
                return std::make_error_code(c);
            };
            io_context().receive(err,io_context().free_space());
            if(err!=std::error_code())
                return;
            else {
                if(auto ser_res = io_context().deserialize(recv_hmsg_);ser_res!=serialization::SerializationEC::NONE)
                {
                    if(ser_res==serialization::SerializationEC::BUFFER_SIZE_LESSER)
                        err = std::make_error_code(std::errc::resource_unavailable_try_again);
                    else{
                        err = std::make_error_code(std::errc::bad_message);
                        MessageHandler<Side::SERVER> err_msg;
                        err_msg.emplace_message_by_id(
                            Message_t<Side::SERVER>::ERROR,
                            ErrorCode::RECEIVING_MESSAGE_ERROR);
                        io_context().serialize(err_msg);
                        io_context().send(err);
                    }
                    return;
                }
            }
            if(receive(sock,hmsg->buffer(),hmsg->buffer().size())==-1){
                err = at_error(std::errc::io_error);
                return;
            }
            else{
                uint64_t data_sz=0;
                if(serialization::deserialize_network(data_sz,std::span<const char>(hmsg->buffer()))!=serialization::SerializationEC::NONE){
                    err = at_error(std::errc::message_size);
                    return;
                }
                hmsg->buffer().resize(data_sz+hmsg->buffer().size(),0);
                if(receive(sock,std::span<char>(hmsg->buffer()).subspan(sizeof(size_t)),hmsg->buffer().size())==-1){
                    err = at_error(std::errc::io_error);
                    return;
                }
                if(serialization::deserialize_network(*hmsg,
                        std::span<const char>(hmsg->buffer()).subspan(sizeof(size_t)))!=
                        serialization::SerializationEC::NONE)
                {
                    err = at_error(std::errc::bad_message);
                    return;
                }
                else{
                    //used for file segments transmission
                    auto has_more_msg = [this](auto&& msg){
                        if constexpr (std::is_same_v<std::decay_t<decltype(msg)>,std::monostate>)
                            this->has_more_ = false;
                        else this->has_more_.exchange(msg.message_more());
                    };
                    std::visit(has_more_msg,*hmsg);
                    return;
                }
            }
            std::lock_guard lk(m_);
            hmsg_ = std::move(hmsg);
            return;
        }
        virtual void on_write(std::error_code& err) noexcept override{
            if(!hmsg || !hmsg->has_message()){
                err = std::make_error_code(std::errc::no_message);
                return;
            }
            err = 
                send(sock,SEND_FLAGS::NoSignal,hmsg->buffer())==-1?
                std::make_error_code(std::errc::io_error):
                std::error_code();
            return;
        }
        virtual void on_task_done(std::error_code& err) noexcept override{

        }
        virtual void on_stop_requested(std::error_code& err) noexcept override{

        }
        
        Process(
                ConnectionHandle hconn,
                std::error_code& err) noexcept:
            AbstractConnectionProcess(hconn,err){}
        ~Process() = default;
        virtual void handle_event(
                    Event event,
                    std::error_code& err) noexcept = 0;
        virtual bool requestable() const noexcept override{
            return false;
        }
    };
    namespace connection::messaging{
        template<>
        class sender<network::Server>{
            std::shared_ptr<MessageHandler<Side::SERVER>> hmsg_;
            std::mutex& m_;
            std::condition_variable& cv_;
            static void __bufferize__(std::error_code& err,
                    std::shared_ptr<MessageHandler<Side::SERVER>> hmsg);
            template<MESSAGE_ID<Side::SERVER>::type T,typename... ARGS>
            std::shared_ptr<MessageHandler<Side::SERVER>> 
                __init_send_message__(std::error_code& err,ARGS... args) noexcept
            {
                auto hmsg  =std::make_shared<decltype(hmsg_)::element_type>();
                hmsg->template emplace_message<T>(std::forward<ARGS>(args)...);
                if(err!=std::error_code())
                    return std::shared_ptr<MessageHandler<Side::SERVER>>();
                __bufferize__(err,hmsg);
                if(err!=std::error_code())
                    return std::shared_ptr<MessageHandler<Side::SERVER>>();
                return hmsg;
            }
            static void __send__(Socket sock, std::error_code& err,
                    std::shared_ptr<MessageHandler<Side::SERVER>>&);
            public:
            sender(std::mutex& mut,std::condition_variable& cv):
                m_(mut),cv_(cv){}
            template<MESSAGE_ID<Side::SERVER>::type T,typename... ARGS>
            void send_message(std::stop_token token,Socket sock,std::error_code& err,ARGS... args) noexcept{
                std::lock_guard lk(m_);
                if(token.stop_requested()){
                    err = std::make_error_code(std::errc::interrupted);
                    return;
                }
                __init_send_message__<T>(err,std::forward<ARGS>(args)...);
                if(token.stop_requested()){
                    err = std::make_error_code(std::errc::interrupted);
                    return;
                }
                __send__(sock,err,hmsg_);
                if(token.stop_requested()){
                    err = std::make_error_code(std::errc::interrupted);
                    return;
                }
                return;
            }
            std::weak_ptr<MessageHandler<Side::SERVER>>
                get_msg() const
            {
                return hmsg_;
            }
        };

        template<>
        class receiver<network::Server>{
            std::shared_ptr<MessageHandler<Side::CLIENT>> hmsg_;
            std::mutex& m_;
            std::condition_variable& cv_;
            std::atomic<bool> has_more_ = false;
            void __receive__(Socket sock, std::error_code& err);
            public:
            receiver(std::mutex& mut,std::condition_variable& cv):
                m_(mut),cv_(cv){}
            void receive_message(Socket sock,std::error_code& err) noexcept{
                __receive__(sock,err);
                cv_.notify_one();
            }
            const std::atomic<bool>& has_more() const{
                return has_more_;
            }
            const std::weak_ptr<const MessageHandler<Side::CLIENT>>
                get_msg() const
            {
                return hmsg_;
            }
        };
    }
}

using namespace network;

namespace network::connection{
    template<>
    class Process<Server>:public EventHandledProcess{
        messaging::sender<Server> sender_;
        messaging::receiver<Server> receiver_;
        public:
        Process(Socket socket,std::condition_variable& cv):
            EventHandledProcess(socket),
            sender_(m_,cv),receiver_(m_,cv){}
        Process(const Process&) = delete;
        Process(Process&& other) = delete;
        Process& operator=(const Process&) = delete;
        Process& operator=(Process&& other) = delete;
        private:
        public:
        static std::optional<typename MESSAGE_ID<Side::CLIENT>::type>
            received_message_type(std::shared_ptr<const 
                    MessageHandler<Side::CLIENT>> msg) noexcept
        {
            if(!msg || !msg->has_message()){
                return std::nullopt;
            }
            else return static_cast<typename MESSAGE_ID<
                Side::CLIENT>::type>(msg->index()-1);
        }
        template<MESSAGE_ID<Side::SERVER>::type T,typename... ARGS>
        void send_message(std::stop_token token,
                    Socket sock,
                    std::error_code& err,
                    ARGS... args) noexcept
        {
            sender_.send_message(token,sock,err,std::forward<ARGS>(args)...);
        }
        void receive_message(Socket sock,std::error_code& err) noexcept
        {
            receiver_.receive_message(sock,err);
        }
        const std::atomic<bool>& has_more() const
        {
            return receiver_.has_more();
        }
        const std::weak_ptr<const MessageHandler<Side::CLIENT>> 
            get_received_msg() const
        {
            return receiver_.get_msg();
        }
        const std::weak_ptr<const MessageHandler<Side::SERVER>>
            get_sending_msg() const
        {
            return sender_.get_msg();
        }
    };
}