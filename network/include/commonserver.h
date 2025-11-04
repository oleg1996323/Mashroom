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
        DERIVED_CONNECTIONPOOL connection_pool_;
        Socket socket_;
        using Interrupted = bool;
        std::future<Interrupted> accept_result;
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
            connection_pool_.add_connection(socket,events_notify);
            return *this;
        }
        CommonServer& remove_connection(const Socket& socket, bool wait){
            remove_connection(socket,wait);
            return *this;
        }
        CommonServer& modify_connection(const Socket& socket,Event events_notify){
            connection_pool_.modify_connection(socket,events_notify);
            return *this;
        }
        public:
        template<typename... CONNPOOL_ARGS>
        CommonServer(CONNPOOL_ARGS&&... args):connection_pool_(std::forward<CONNPOOL_ARGS>(args)...){}
        template<typename... CONNPOOL_ARGS>
        CommonServer(const server::Settings& settings,CONNPOOL_ARGS&&... args):
        socket_(Socket(settings.host_,settings.port_,Socket::Type::Stream,settings.protocol_)),
        connection_pool_(std::forward<CONNPOOL_ARGS>(args)...){
            if(settings.reuse_address_)
                socket_.set_option(network::Socket::Option<int>(1,Socket::Options::ReuseAddress));
            socket_.bind();
        }
        virtual ~CommonServer(){
            if(accepter)
                accepter->interrupt();
            connection_pool_.stop(false);
            accept_result.wait();
        }
        template<typename... ARGS>
        void configure(const server::Settings& settings, Socket::Option<ARGS>... options){
            if(!is_launched()){
                socket_ = Socket(settings.host_,settings.port_,Socket::Type::Stream,settings.protocol_);
                socket_.set_options(std::forward<decltype(options)>(options)...);
                socket_.bind();
            }
            else{
                throw std::runtime_error("Server is not stopped for further configuration!");
            }
        }
        virtual void before_listen(){};
        void listen(int backlog){
            before_listen();
            assert(socket_.is_valid());
            if(::listen(socket_.__descriptor__(),backlog)==-1)
                __listen_throw__();
            after_listen();
        }
        virtual void after_listen(){};
        virtual void before_accept(){
            std::cout<<"server: waiting for connections…"<<std::endl;
        };
        void accept();
        virtual void after_accept(Socket& accepted){};
        const Socket socket() const{
            return socket_;
        }
        virtual void at_closing(){}
        virtual void at_collapsing(){}
        void close(bool wait_for_end_connections = false, uint16_t timeout_sec = 60);
        void collapse(bool wait_for_end_connections = false, uint16_t timeout_sec = 60);
        void launch(){
            if(!socket_.is_valid())
                throw std::runtime_error("Server not configured");
            listen(5);
            try{
                accepter = std::move(std::make_unique<Multiplexor>(5+1));
                accepter->add(socket_,Multiplexor::Event::In|Multiplexor::Event::HangUp|Multiplexor::Event::EdgeTrigger);
            }
            catch(...){
                using namespace std::string_literals;
                accepter.reset();
                throw std::runtime_error("server epoll init: "s+strerror(errno));
            }
            accept();
        }
        bool is_launched() const{
            return socket_.is_valid() && accepter;
        }
        const DERIVED_CONNECTIONPOOL& get_connection_pool() const{
            return connection_pool_;
        }
    };
}

namespace network{
    template<typename DERIVED_CONNECTIONPOOL>
    void CommonServer<DERIVED_CONNECTIONPOOL>::__accept_throw__(const Socket& socket){
        int err = errno;
        errno = 0;
        switch(err){
            #ifdef EAGAIN
            case EAGAIN:
            #elif defined EWOULDBLOCK
            case EWOULDBLOCK:
            #endif
            case EINTR:
                break;
            case ECONNABORTED:
                connection_aborted(err,socket);
                break;
            case EMFILE:
                process_max_fd_reached(err,socket);
            case ENFILE:
                system_max_fd_reached(err,socket);
                break;
            case EPERM:
                break;
            case EBADF:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
            case EPROTO:
            case EFAULT:
                throw std::runtime_error(strerror(err));
            case EINVAL:
                throw std::invalid_argument(strerror(err));
        }
    }

    template<typename DERIVED_CONNECTIONPOOL>
    void CommonServer<DERIVED_CONNECTIONPOOL>::accept(){
        accept_result= std::async(std::launch::async,[this](){
            connection_pool_.polling_connections();
            socklen_t sin_size = address_struct_size(*socket_.storage);
            for(;;){
                if (accepter->interrupted()){
                    accepter.reset();
                    return true;
                }
                try{
                    assert(socket_.is_valid());
                    for(auto event: accepter->wait(-1)){
                        if(event.data.fd==socket_.__descriptor__()){
                            sockaddr_storage another;
                            memset(&another,0,sizeof(sockaddr_storage));
                            try{
                                before_accept();
                                if(int raw_sock = ::accept(socket_.__descriptor__(),(sockaddr*)&another,&sin_size);raw_sock==-1){
                                    Socket sock(raw_sock,another);
                                    #ifdef DEBUG
                                    std::cout<<"Connection refused: ";
                                    sock.print_address_info(std::cout);
                                    #endif
                                    __accept_throw__(sock);
                                    std::cout<<strerror(errno)<<std::endl;
                                    errno = 0;
                                    continue;
                                }
                                else{
                                    Socket socket(raw_sock,another);
                                    using Event_t = Multiplexor::Event;
                                    connection_pool_.add_connection(socket,Event_t::HangUp|Event_t::In);
                                    after_accept(socket);
                                    continue;
                                }
                            }
                            catch(const std::runtime_error& err){
                                std::cout<<err.what()<<std::endl;
                                continue;
                            }
                        }
                        else{
                            if(connection_pool_.contains_socket(event.data.fd))
                                connection_pool_.get_socket(event.data.fd);
                        }
                    }
                }
                catch(const std::exception& err){
                    std::cout<<err.what()<<std::endl;
                    return false;
                }
            }
            return false;
        });
    }

    template<typename DERIVED_CONNECTIONPOOL>
    void CommonServer<DERIVED_CONNECTIONPOOL>::close(bool wait_for_end_connections, uint16_t timeout_sec){
        
        if(accepter)
            accepter->interrupt();
        connection_pool_.stop(wait_for_end_connections,timeout_sec);
        accept_result.wait();
    }
    template<typename DERIVED_CONNECTIONPOOL>
    void CommonServer<DERIVED_CONNECTIONPOOL>::collapse(bool wait_for_end_connections, uint16_t timeout_sec){
        //@todo Block sockets maybe?
    }
}