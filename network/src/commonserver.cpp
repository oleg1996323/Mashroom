#include "commonserver.h"

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
    CommonServer<DERIVED_CONNECTIONPOOL>::Interrupted CommonServer<DERIVED_CONNECTIONPOOL>::accept(){
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

    template<typename DERIVED_CONNECTIONPOOL>
    void CommonServer<DERIVED_CONNECTIONPOOL>::close(bool wait_for_end_connections, uint16_t timeout_sec){
        connection_pool.stop(wait_for_end_connections,timeout_sec);
    }
    template<typename DERIVED_CONNECTIONPOOL>
    void CommonServer<DERIVED_CONNECTIONPOOL>::collapse(bool wait_for_end_connections, uint16_t timeout_sec){
        connection_pool.stop(wait_for_end_connections,timeout_sec);
        //@todo Block sockets maybe?
    }
}