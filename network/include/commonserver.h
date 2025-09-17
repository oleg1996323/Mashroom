#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <thread>
#include <sys/eventfd.h>
#include "definitions.h"
#include "serversettings.h"
#include "abstractconnectionpool.h"
#include "concepts.h"
#include "commonsocket.h"
#include "multiplexor.h"
#include <stdexcept>
#include <set>

namespace network{
    template<typename DERIVED_CONNECTIONPOOL>
    class CommonServer{
        std::unique_ptr<Multiplexor> accepter;
        DERIVED_CONNECTIONPOOL connection_pool;
        Socket socket_;
        using Interrupted = bool;
        void __listen_throw__(){
            int err = errno;
            errno = 0;
            throw std::runtime_error(strerror(err));
        }
        void __accept_throw__(const Socket& socket);
        public:
        using Event = Multiplexor::Event;
        protected:
        virtual void process_max_fd_reached(int err, const Socket& socket){
            std::cout<<strerror(err)<<std::endl;
        }
        virtual void system_max_fd_reached(int err,const Socket& socket){
            std::cout<<strerror(err)<<std::endl;
        }
        virtual void connection_aborted(int err,const Socket& socket){
            std::cout<<strerror(err)<<std::endl;
        }
        CommonServer& add_connection(const Socket& socket, Event events_notify){
            connection_pool.add_connection(socket,events_notify);
            return *this;
        }
        CommonServer& remove_connection(const Socket& socket, bool wait){
            remove_connection(socket,wait);
            return *this;
        }
        CommonServer& modify_connection(const Socket& socket,Event events_notify){
            connection_pool.modify_connection(socket,events_notify);
            return *this;
        }
        public:
        template<typename... ARGS>
        CommonServer(const server::Settings& settings,ARGS&&... args):
        socket_(Socket(settings.host,settings.port,Socket::Type::Stream,Protocol::TCP)),
        connection_pool(std::forward<ARGS>(args)...){
            socket_.bind();
        }
        virtual ~CommonServer(){}
        void stop(bool wait_finish) {
            accepter->interrupt();
            connection_pool.stop(wait_finish);
        }
        virtual void before_listen(){};
        void listen(int backlog){
            if(::listen(socket_.__descriptor__(),backlog)==-1)
                __listen_throw__();
        }
        virtual void after_listen(const Socket& accepted){};
        virtual void before_accept(){
            std::cout<<"server: waiting for connections…"<<std::endl;
        };
        Interrupted accept();
        virtual void after_accept(Socket& accepted){};
        virtual void at_close(){
            connection_pool.stop(false);
        }
        const Socket socket() const{
            return socket_;
        }
        void close(bool wait_for_end_connections = false, uint16_t timeout_sec = 60);
        void collapse(bool wait_for_end_connections = false, uint16_t timeout_sec = 60);
        void launch(){
            if(socket_.is_valid())
                return;
            before_listen();
            listen(5);
            try{
                accepter = std::make_unique<Multiplexor>(5+1);
                accepter->add(socket_,Multiplexor::Event::In|Multiplexor::Event::HangUp|Multiplexor::Event::EdgeTrigger);
            }
            catch(...){
                using namespace std::string_literals;
                throw std::runtime_error("server epoll init: "s+strerror(errno));
            }
            if(accept()==true){
                accepter.reset();
                return;
            }
        }
    };
}