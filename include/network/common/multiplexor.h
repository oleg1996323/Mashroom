#pragma once
#include "def.h"
#include "socket.h"
#include <vector>

namespace network{
    /**
     * @public Add/remove/modify network::Socket
     */
    class Multiplexor{
        std::vector<epoll_event> events_;
        FileDescriptor epollfd = -1;
        void __epoll_ctl_throw__(){
            int err = errno;
            errno = 0;
            switch(err){
                case EPERM:
                case ENOMEM:
                case EBADF:
                    throw std::runtime_error(strerror(err));
                    break;
                case EEXIST:
                case EINVAL:
                case ENOENT:
                    throw std::invalid_argument(strerror(err));
                    break;
                case ELOOP:
                case ENOSPC:
                    throw std::logic_error(strerror(err));
                    break;
                default:
                    throw std::runtime_error(strerror(err));
                break;
            }
        }

        void __epoll_wait_throw__(){
            int err = errno;
            errno = 0;
            switch(errno){
                case EBADF:
                case EFAULT:
                throw std::runtime_error(strerror(err));
                    break;
                case EINTR:
                //ignore interruptions
                    break;
                case EINVAL:
                    throw std::invalid_argument(strerror(err));
                default:
                    throw std::runtime_error(strerror(err));
            }
        }

    public:
        using Event_t = epoll_event;
        enum Event{
            /**
             * @brief The associated file is available for read() operations.
             */            
            In = EPOLLIN,
            /**
             * @brief The associated file is available for write() operations.
             */
            Out = EPOLLOUT,
            /**
             * @brief There is an exceptional condition on the file descriptor.
             * @details There is some exceptional condition on the file descriptor.
                @Possibilities include:
                -There is out-of-band data on a TCP socket (see tcp(7)).
                -A pseudoterminal master in packet mode has seen a state
                change on the slave (see ioctl_tty(2)).
                -A cgroup.events file has been modified (see cgroups(7)).
             */
            HighProrityIn = EPOLLPRI,
            /**
             * @brief Requests edge-triggered notification for the associated
              file descriptor.  The default behavior for epoll is level-
              triggered.  See epoll() for more detailed information
              about edge-triggered and level-triggered notification.
             */
            EdgeTrigger = EPOLLET,
            /**
             * @brief Only once the file-descriptor's event will be notified.
             * Further event-notifying of the fd needs the rearming by modify() method.
             */
            OneShot = EPOLLONESHOT,
            Error = EPOLLERR,
            #ifdef EPOLLWAKEUP
            /**
             * @brief Ensure that the system does not enter "suspend" or "hibernate" while this event is
              pending or being processed.
             */
            WakeUp = EPOLLWAKEUP,
            #endif
            #ifdef EPOLLEXCLUSIVE
            /**
             * @brief In multi-threads/processes applications exclude the event notifying
             * in all multiplexors (epoll).
             */
            Exclusive = EPOLLEXCLUSIVE,
            #endif
            /**
             * @brief 
             */
            HangUp = EPOLLHUP,
            CanReadButHangUp = EPOLLRDHUP
        };
        Multiplexor(size_t mp_controled):
        events_([mp_controled](){
            std::vector<epoll_event> res;
            res.resize(mp_controled);
            return res;
        }()),
        epollfd(epoll_create(mp_controled)){}

        Multiplexor& add(const Socket& socket,Event event){
            epoll_event ev{.events = event,.data = epoll_data_t{.fd = socket.__descriptor__()}};
            if(epoll_ctl(epollfd,EPOLL_CTL_ADD,socket.__descriptor__(),&ev)==-1)
                __epoll_ctl_throw__();
        }
        Multiplexor& remove(const Socket& socket){
            if(epoll_ctl(epollfd,EPOLL_CTL_DEL,socket.__descriptor__(),nullptr)==-1)
                __epoll_ctl_throw__();
        }
        Multiplexor& modify(const Socket& socket,Event event){
            epoll_event ev{.events = event,.data = epoll_data_t{.fd = socket.__descriptor__()}};
            if(epoll_ctl(epollfd,EPOLL_CTL_MOD,socket.__descriptor__(),&ev)==-1)
                __epoll_ctl_throw__();
        }
        /**
         * @param timeout - in milliseconds
         * If timeout == -1 than infinite timeout is set.
         */
        std::span<Event_t> wait(int32_t timeout){
            if(int event_sz = epoll_wait(epollfd,events_.data(),events_.size(),timeout);event_sz==-1)
                __epoll_wait_throw__();
            else return std::span(events_).subspan(event_sz);
        }
    };
}

network::Multiplexor::Event operator|(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)|
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}
network::Multiplexor::Event operator&(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)&
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}
network::Multiplexor::Event operator^(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)^
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}
network::Multiplexor::Event operator~(network::Multiplexor::Event val) noexcept{
    return static_cast<network::Multiplexor::Event>(~static_cast<std::underlying_type_t<network::Multiplexor::Event>>(val));
}
network::Multiplexor::Event operator&(network::Multiplexor::Event lhs,int rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)&
            rhs);
}
network::Multiplexor::Event operator&(std::underlying_type_t<network::Multiplexor::Event> lhs, network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(lhs &
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}
network::Multiplexor::Event operator|(network::Multiplexor::Event lhs,std::underlying_type_t<network::Multiplexor::Event> rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(static_cast<std::underlying_type_t<network::Multiplexor::Event>>(lhs)|
            rhs);
}
network::Multiplexor::Event operator|(std::underlying_type_t<network::Multiplexor::Event> lhs,network::Multiplexor::Event rhs) noexcept{
    return static_cast<network::Multiplexor::Event>(lhs |
            static_cast<std::underlying_type_t<network::Multiplexor::Event>>(rhs));
}