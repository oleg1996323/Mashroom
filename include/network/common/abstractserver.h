#pragma once
#include <netinet/in.h>
#include <unistd.h>
#include <list>
#include <thread>
#include <sys/eventfd.h>
#include "network/common/def.h"
#include "sys/config/server.h"
#include "network/server/connection_pool.h"
#include "concepts.h"
#include "network/common/socket.h"
#include "network/common/multiplexor.h"
#include <stdexcept>

namespace network{
    enum ServerError{
        NONE,
        INTERRUPTED,
        ACCEPTER
    };
    class AbstractServer{
        Multiplexor accepter;
        connection::AbstractConnectionPool connection_pool;
        eventfd_t server_interruptor;
        Socket socket_;
        ServerError err_;

        void __listen_throw__(){
            int err = errno;
            errno = 0;
            throw std::runtime_error(strerror(err));
        }
        void __accept_throw__(const Socket& socket){
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
        public:
        AbstractServer(const server::Settings& settings):
        accepter(5+1),
        socket_(Socket(settings.host,settings.port,Socket::Type::Stream,Protocol::TCP)){
            accepter.add(socket_,Multiplexor::Event::In);
        }
        virtual ServerError before_init() = 0;
        virtual ServerError init(const std::vector<std::string>& args){};
        virtual ServerError after_init() = 0;
        virtual ServerError before_connect() = 0;
        ServerError connect();
        virtual ServerError after_connect() = 0;
        virtual ServerError before_listen() = 0;
        ServerError listen(int backlog){
            if(::listen(socket_.__descriptor__(),backlog)==-1)
                __listen_throw__();
        }
        virtual ServerError after_listen(const Socket& accepted) = 0;
        virtual ServerError before_accept(){};
        ServerError accept_process();
        ServerError accept_thread(){
            for(;;){
                int number_epoll_wait = 0;
                try{
                    for(auto& event: accepter.wait(-1)){
                        if (event.data.fd == server_interruptor){
                            uint64_t val;
                            read(server_interruptor, &val, sizeof(val));
                            return ServerError::INTERRUPTED;
                        }
                        else if(event.data.fd==socket_.__descriptor__()){
                            socklen_t sin_size;
                            sockaddr_storage another;
                            memset(&another,0,sizeof(sockaddr_storage));
                            try{
                                if(before_accept()!=ServerError::NONE)
                                    continue;
                                if(int raw_sock = ::accept(socket_.__descriptor__(),(sockaddr*)&another,&sin_size);raw_sock==-1){
                                    __accept_throw__(Socket(raw_sock,another));
                                    std::cout<<strerror(errno)<<std::endl;
                                    errno = 0;
                                    continue;
                                }
                                else{
                                    Socket socket(raw_sock,another);
                                    if(after_accept(socket)!=ServerError::NONE)
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
                    return ServerError::ACCEPTER;
                }
            }
        }
        virtual ServerError after_accept(Socket& accepted){};
        virtual ServerError at_close() = 0;
        const std::set<Socket>& peers_list() const;
        const Socket socket() const;
        void close();
        void collapse();
        void launch(){
            if(socket_.is_valid())
                return;
            if(listen(5)!=ServerError::NONE)
                throw std::runtime_error(strerror(errno));
            
            std::vector<epoll_event> events = server::define_epoll_event();
            epoll_event ev;
            ev.events = EPOLLRDNORM | EPOLLRDHUP | EPOLLWRNORM;
            ev.data.fd = socket_.__descriptor__();
            /**
             * @todo set correct size of accepted epoll
             */
            int epollfd = epoll_create(20);
            if(epollfd==-1)
                ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"server epoll init. "s+strerror(errno),AT_ERROR_ACTION::ABORT);
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socket_.__descriptor__(), &ev) == -1){
                perror("epoll_ctl: server socket");
                throw std::runtime_error(strerror(errno));
            }
            server_interruptor = eventfd(0,EFD_NONBLOCK);
            if(server_interruptor==-1)
                throw std::runtime_error(strerror(errno));
            ev.data.fd = server_interruptor;
            ev.events = EPOLLIN;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_interruptor, &ev) == -1) {
                perror("epoll_ctl: eventfd");
                ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR, strerror(errno), AT_ERROR_ACTION::CONTINUE);
                ::close(epollfd);
                ::close(server_interruptor);
                return;
            }
            printf("server: waiting for connections…\n");
        }
        //fork
        //thread
    };
}