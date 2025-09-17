#pragma once
#include "definitions.h"
#include "commonsocket.h"
#include <vector>
#include <sys/epoll.h>
#include <span>
#include <sys/eventfd.h>
#include <set>

namespace network{
    /**
     * @public Add/remove/modify network::Socket
     */
    class Multiplexor{
        class Interruptor{
            friend Multiplexor;
            eventfd_t fd_=-1;
            bool interrupted = false;
            Interruptor(eventfd_t fd):fd_(fd){}
            public:
            ~Interruptor(){
                close(fd_);
            }
        };

        class Semaphore{
            friend Multiplexor;
            eventfd_t fd_;
            Semaphore(eventfd_t fd):fd_(fd){}
        };

        using CustomEvent = std::variant<Interruptor,Semaphore>;

        std::vector<epoll_event> events_;
        std::unique_ptr<Interruptor> interruptor;
        FileDescriptor epollfd = -1;
        void __epoll_ctl_throw__();
        void __epoll_wait_throw__();
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
    private:
        void __set_interruptor__();
    public:
        Multiplexor(size_t mp_controled);
        ~Multiplexor();
        Multiplexor& add(const Socket& socket,Event event);
        Multiplexor& remove(const Socket& socket);
        Multiplexor& modify(const Socket& socket,Event event);
        void interrupt() noexcept;
        bool interrupted() noexcept;
        /**
         * @param timeout - in milliseconds
         * If timeout == -1 than infinite timeout is set.
         */
        std::span<Event_t> wait(int32_t timeout);
    };
}

network::Multiplexor::Event operator|(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept;
network::Multiplexor::Event operator&(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept;
network::Multiplexor::Event operator^(network::Multiplexor::Event lhs,network::Multiplexor::Event rhs) noexcept;
network::Multiplexor::Event operator~(network::Multiplexor::Event val) noexcept;
network::Multiplexor::Event operator&(network::Multiplexor::Event lhs,int rhs) noexcept;
network::Multiplexor::Event operator&(std::underlying_type_t<network::Multiplexor::Event> lhs, network::Multiplexor::Event rhs) noexcept;
network::Multiplexor::Event operator|(network::Multiplexor::Event lhs,std::underlying_type_t<network::Multiplexor::Event> rhs) noexcept;
network::Multiplexor::Event operator|(std::underlying_type_t<network::Multiplexor::Event> lhs,network::Multiplexor::Event rhs) noexcept;