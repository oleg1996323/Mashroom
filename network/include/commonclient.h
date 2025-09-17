#pragma once
#include <unistd.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <variant>
#include <netdb.h>
#include "commonsocket.h"
#include "abstractprocess.h"

namespace network{
    class ClientsHandler;
}
namespace network{

    template<typename PROCESS_T>
    class CommonClient{
        void __connect_throw__();
        
        void __connect_configured__(){
            if(socket_ && socket_->get_address_storage()){
                if(::connect(*(socket_->socket_.get()),reinterpret_cast<sockaddr*>(socket_->get_address_storage().get()),sizeof(sockaddr_storage))==-1)
                    __connect_throw__();
            }
        }
        protected:
        /**
         * @brief don't block the GUI if integrated
         */
        std::unique_ptr<PROCESS_T> process;
        std::unique_ptr<Socket> socket_;
        using Process = decltype(process)::element_type;
        public:
        CommonClient(const std::string& host, Port port):
        socket_(std::make_unique<Socket>(host,port,Socket::Type::Stream,Protocol::TCP)){}
        CommonClient(CommonClient&& other) noexcept:
        process(std::move(other.process)),
        socket_(std::move(other.socket_))
        {}
        CommonClient& operator=(CommonClient&& other) noexcept{
            if(this!=&other){
                process = std::move(other.process);
                socket_ = std::move(other.socket_);
            }
            return *this;
        }
        bool operator==(const CommonClient& other) = delete;
        ~CommonClient(){
            disconnect(false,0);            
        }
        void cancel(){
            if(process && process->busy())
                process->request_stop(false);
        }
        virtual CommonClient& before_connect(){return *this;}
        CommonClient& connect(const std::string& host, const Port port){
            if(process && process->ready()){
                process.reset();
                socket_ = std::make_unique<Socket>(host,port,Socket::Type::Stream,Protocol::TCP);
            }
            if(::connect(*(socket_->socket_.get()),reinterpret_cast<sockaddr*>(socket_->get_address_storage().get()),sizeof(sockaddr_storage))==-1)
                    __connect_throw__();
        }
        virtual CommonClient& after_connect(const Socket&){return *this;}
        bool is_connected() const{
            return socket_ && socket_->is_connected();
        }
        virtual CommonClient& before_disconnect(const Socket&){return *this;}
        CommonClient& disconnect(bool wait_finish, uint16_t timeout_sec = 60){
            if(is_connected() && process && process->busy())
                process->request_stop(wait_finish,timeout_sec);
            socket_.reset();
        }
        virtual CommonClient& after_disconnect(){return *this;}
        template<typename... ARGS>
        void add_request(typename PROCESS_T::Function_t<ARGS...> func,ARGS&&... args) const{
            if(socket_ && process && process->busy())
                process->action_if_process_busy(std::move(func),*socket_,std::forward<ARGS>(args)...);
            else process = PROCESS_T::add_process(std::move(func),*socket_,std::forward<ARGS>(args)...);
        }
    };
}