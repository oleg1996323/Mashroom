#pragma once
#include <unistd.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <variant>
#include <netdb.h>
#include "socket.h"
#include "abstractprocess.h"

namespace network{
    class ClientsHandler;
}
namespace network{

    template<typename PROCESS_T>
    class AbstractClient{
        static_assert(std::is_base_of_v<AbstractProcess<PROCESS_T>,PROCESS_T>,
            "Template argument must be derived from network::AbstractProcess class!");

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
        using Process = decltype(process)::element_type;
        std::unique_ptr<Socket> socket_;
        AbstractClient(const std::string& host, Port port):
        socket_(std::make_unique<Socket>(host,port,Socket::Type::Stream,Protocol::TCP)){}
        public:
        AbstractClient(AbstractClient&& other) noexcept;
        AbstractClient& operator=(AbstractClient&& other) noexcept;
        bool operator==(const AbstractClient& other) const noexcept;
        ~AbstractClient();
        void cancel(){

        }
        virtual AbstractClient& before_connect(){return *this;}
        AbstractClient& connect(const std::string& host, const std::string& port){
            ::connect(socket_, (sockaddr*)socket_.get(), sizeof(sockaddr)) == -1
        }
        static std::unique_ptr<Client> create_and_connect(const std::string& host, uint16_t port){
            std::unique_ptr<Client> client = std::make_unique<Client>(host,port);

        }
        virtual AbstractClient& after_connect(const Socket&){return *this;}
        bool is_connected() const{
            return socket_ && socket_->is_connected();
        }
        virtual AbstractClient& before_disconnect(const Socket&){return *this;}
        AbstractClient& disconnect(bool wait_finish, uint16_t timeout_sec = 60){
            if(is_connected() && process && process->busy())
                process->request_stop(wait_finish,timeout_sec);
            socket_.reset();
        }
        virtual AbstractClient& after_disconnect(){return *this;}
        template<typename... ARGS>
        void add_request(PROCESS_T::Function_t function,ARGS&&... args) const{
            if(socket_ && process && process->busy())
                process->action_if_process_busy(std::move(function),*socket_,std::forward<ARGS>(args)...);
            else process = PROCESS_T::add_process(std::move(function),*socket_,std::forward<ARGS>(args)...);
        }
    };
}