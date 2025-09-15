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
#include "socket.h"
#include "multiplexor.h"
#include <stdexcept>
#include <set>

namespace network{
    template<typename DERIVED_CONNECTIONPOOL>
    class AbstractServer{
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
        AbstractServer& add_connection(const Socket& socket, Event events_notify){
            connection_pool.add_connection(socket,events_notify);
            return *this;
        }
        AbstractServer& remove_connection(const Socket& socket, bool wait){
            remove_connection(socket,wait);
            return *this;
        }
        AbstractServer& modify_connection(const Socket& socket,Event events_notify){
            connection_pool.modify_connection(socket,events_notify);
            return *this;
        }
        public:
        AbstractServer(const server::Settings& settings):
        socket_(Socket(settings.host,settings.port,Socket::Type::Stream,Protocol::TCP)){
            socket_.bind();
        }
        void stop(bool wait_finish) {
            accepter->interrupt();
            connection_pool.stop(wait_finish);
        }
        virtual void init(const std::vector<std::string>& args){};
        virtual void before_listen(){};
        void listen(int backlog){
            if(::listen(socket_.__descriptor__(),backlog)==-1)
                __listen_throw__();
        }
        virtual void after_listen(const Socket& accepted) = 0;
        virtual void before_accept(){
            printf("server: waiting for connections…\n");
        };
        Interrupted accept(){
            for(;;){
                int number_epoll_wait = 0;
                try{
                    for(auto& event: accepter->wait(-1)){
                        if (accepter->interrupted())
                            return true;
                        else if(event.data.fd==socket_.__descriptor__()){
                            socklen_t sin_size;
                            sockaddr_storage another;
                            memset(&another,0,sizeof(sockaddr_storage));
                            try{
                                before_accept();
                                if(int raw_sock = ::accept(socket_.__descriptor__(),(sockaddr*)&another,&sin_size);raw_sock==-1){
                                    __accept_throw__(Socket(raw_sock,another));
                                    std::cout<<strerror(errno)<<std::endl;
                                    errno = 0;
                                    continue;
                                }
                                else{
                                    Socket socket(raw_sock,another);
                                    after_accept(socket);
                                    using Event_t = Multiplexor::Event;
                                    connection_pool.add_connection(socket,Event_t::HangUp|Event_t::In|Event_t::Out);
                                    continue;
                                }
                            }
                            catch(const std::runtime_error& err){
                                std::cout<<err.what()<<std::endl;
                                continue;
                            }
                        }
                        else{
                            if(connection_pool.contains_socket(event.data.fd))
                                connection_pool.get_socket(event.data.fd);
                        }
                    }
                }
                catch(const std::exception& err){
                    std::cout<<err.what()<<std::endl;
                    return false;
                }
                return false;
            }
        }
        virtual void after_accept(Socket& accepted){};
        virtual void at_close(){
            connection_pool.stop();
        }
        const std::set<Socket>& peers_list() const;
        const Socket socket() const;
        void close();
        void collapse();
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