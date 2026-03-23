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
#include "abstractqueuableprocess.h"

namespace network{
    void reply(std::stop_token stop,const Socket& socket,connection::Process<Server>* proc);

    class Server;
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