#pragma once
#include <span>
#include <vector>
#include <netdb.h>
#include "definitions.h"
#include <cstring>
#include <ranges>
#include <type_traits>
#include <cstdint>
#include "commonsocket.h"
#include <iostream>

namespace network{
    enum class SEND_FLAGS:int{
        /**
         * @brief Tell the link layer that forward progress happened: you got
              a successful reply from the other side.  If the link layer
              doesn't get this it will regularly reprobe the neighbor
              (e.g., via a unicast ARP).  Valid only on SOCK_DGRAM and
              SOCK_RAW sockets and currently implemented only for IPv4
              and IPv6.  See arp(7) for details.
         */
        Confirm = MSG_CONFIRM,
        /**
         * @brief Don't use a gateway to send out the packet, send to hosts
              only on directly connected networks.  This is usually used
              only by diagnostic or routing programs.  This is defined
              only for protocol families that route; packet sockets
              don't.
         */
        DontRoute = MSG_DONTROUTE,
        /**
         * @brief Enables nonblocking operation; if the operation would
              block, EAGAIN or EWOULDBLOCK is returned.  This provides
              similar behavior to setting the O_NONBLOCK flag (via the
              fcntl(2) F_SETFL operation), but differs in that
              MSG_DONTWAIT is a per-call option, whereas O_NONBLOCK is a
              setting on the open file description (see open(2)), which
              will affect all threads in the calling process as well as
              other processes that hold file descriptors referring to the
              same open file description.
         */
        DontWait = MSG_DONTWAIT,
        /**
         * @brief Terminates a record (when this notion is supported, as for
              sockets of type SOCK_SEQPACKET).
         */
        EndOfRecord = MSG_EOR,
        /**
         * @brief The caller has more data to send.  This flag is used with
              TCP sockets to obtain the same effect as the TCP_CORK
              socket option (see tcp(7)), with the difference that this
              flag can be set on a per-call basis.

              Since Linux 2.6, this flag is also supported for UDP
              sockets, and informs the kernel to package all of the data
              sent in calls with this flag set into a single datagram
              which is transmitted only when a call is performed that
              does not specify this flag.  (See also the UDP_CORK socket
              option described in udp(7).)
         */
        More = MSG_MORE,
        /**
         * @brief Don't generate a SIGPIPE signal if the peer on a stream-
              oriented socket has closed the connection.  The EPIPE error
              is still returned.  This provides similar behavior to using
              sigaction(2) to ignore SIGPIPE, but, whereas MSG_NOSIGNAL
              is a per-call feature, ignoring SIGPIPE sets a process
              attribute that affects all threads in the process.
         */
        NoSignal = MSG_NOSIGNAL,
        /**
         * @brief Sends out-of-band data on sockets that support this notion
              (e.g., of type SOCK_STREAM); the underlying protocol must
              also support out-of-band data.
         */
        OutOfBand = MSG_OOB,
        /**
         * @brief Attempts TCP Fast Open (RFC7413) and sends data in the SYN
              like a combination of connect(2) and write(2), by
              performing an implicit connect(2) operation.  It blocks
              until the data is buffered and the handshake has completed.
              For a non-blocking socket, it returns the number of bytes
              buffered and sent in the SYN packet.  If the cookie is not
              available locally, it returns EINPROGRESS, and sends a SYN
              with a Fast Open cookie request automatically.  The caller
              needs to write the data again when the socket is connected.
              On errors, it sets the same errno as connect(2) if the
              handshake fails.  This flag requires enabling TCP Fast Open
              client support on sysctl net.ipv4.tcp_fastopen.

              Refer to TCP_FASTOPEN_CONNECT socket option in tcp(7) for
              an alternative approach.
         */
        FastOpen = MSG_FASTOPEN
    };
    
    int send(const Socket& socket,SEND_FLAGS flags,const std::ranges::random_access_range auto&... buffers) noexcept requires((sizeof(decltype(buffers))+...)>0){
        auto send_sequentialy = [&](auto&& buffer,SEND_FLAGS flags) noexcept -> bool
        {
            size_t sent = 0;
            if(buffer.size()==0)
                return true;
            while(sent<buffer.size()){
                ssize_t cur = ::send(socket.__descriptor__(),buffer.data()+sent,buffer.size()-sent,static_cast<int>(flags));
                if(cur==-1)
                    return false;
                else sent+=cur;
                if(sent<buffer.size())
                    std::cout<<"Sending resting message"<<std::endl;
            }
            return true;
        };

        size_t i = 0;
        constexpr size_t count = sizeof...(buffers);
        auto send_with_flags = [&](auto&& buf) noexcept{
            return send_sequentialy(buf,flags);
        };

        if(!(send_with_flags(buffers) && ...)) {
            return -1;
        }
        return 0;
    }

    int send(const Socket& socket,const std::ranges::random_access_range auto&... buffers) noexcept{
        return send(socket,static_cast<SEND_FLAGS>(0),buffers...);
    }
}