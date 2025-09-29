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
        virtual void before_connect(){}
        virtual void after_connect(const Socket&){}
        std::unique_ptr<PROCESS_T> process;
        std::unique_ptr<Socket> socket_;
        using Process = decltype(process)::element_type;
        public:
        CommonClient() = default;
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
            disconnect(false,0).socket_.reset();            
        }
        CommonClient& cancel(){
            if(process && process->busy())
                process->request_stop(false);
        }
        
        CommonClient& connect(const std::string& host, const Port port){
            if(process && process->ready()){
                process.reset();
                socket_ = std::make_unique<Socket>(host,port,Socket::Type::Stream,Protocol::TCP);
            }
            if(!socket_)
                socket_ = std::move(std::make_unique<Socket>(host,port,Socket::Type::Stream,Protocol::TCP));

            try{
                before_connect();
                ::connect(*(socket_->socket_.get()),reinterpret_cast<sockaddr*>(socket_->get_address_storage().get()),sizeof(sockaddr_storage));
                    // __connect_throw__();
                after_connect(*socket_);
            }
            catch(const std::exception& err){
                std::cout<<err.what()<<std::endl;
            }
            return *this;
        }
        CommonClient& connect(){
            if(socket_ && !is_connected() && socket_->is_valid()){
                if(::connect(*(socket_->socket_.get()),reinterpret_cast<sockaddr*>(socket_->get_address_storage().get()),sizeof(sockaddr_storage))==-1)
                    __connect_throw__();
                else return *this;
            }
            else throw std::runtime_error(strerror(EISCONN));
            return *this;
        }
        
        bool is_connected() const{
            return socket_ && socket_->is_connected();
        }
        virtual CommonClient& before_disconnect(const Socket&){return *this;}
        CommonClient& disconnect(bool wait_finish, uint16_t timeout_sec = 60){
            if(is_connected() && process && process->busy())
                process->request_stop(wait_finish,timeout_sec);
            return *this;
        }
        virtual CommonClient& after_disconnect(){return *this;}
        template<typename F,typename... ARGS>
        void add_request(F&& func,ARGS&&... args){
            if(socket_ && process && process->busy())
                process->action_if_process_busy();
            else process = PROCESS_T::add_process(std::move(func),*socket_,std::forward<ARGS>(args)...);
        }
    };
}


namespace network{
    template<typename PROCESS_T>
    void CommonClient<PROCESS_T>::__connect_throw__(){
        int err = errno;
        errno = 0;
        switch(err){
            case EACCES:
            case EPERM:
            case EADDRINUSE:
            case EAGAIN:
            case EALREADY:
            case EBADF:
            case ECONNREFUSED:
            case EFAULT:
            case EINPROGRESS:
            case EISCONN:
            case ENETUNREACH:
            case ENOTSOCK:
            case ETIMEDOUT:
                throw std::runtime_error(strerror(err));
                break;
            case EADDRNOTAVAIL:
            case EAFNOSUPPORT:
            case EPROTOTYPE:
                throw std::invalid_argument(strerror(err));
                break;
            case EINTR:
                return;
                break;
            default:
                break;
            return;
        }
    }
}