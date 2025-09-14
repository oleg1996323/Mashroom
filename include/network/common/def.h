#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>

#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/sendfile.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <thread>

namespace network{
    enum class Side{
        SERVER,
        CLIENT
    };

    template<Side S>
    constexpr Side sent_from(){
        if constexpr(S == Side::SERVER)
            return Side::CLIENT;
        else return Side::SERVER;
    }

    namespace server{
        static std::vector<struct epoll_event> define_epoll_event(){
            std::vector<struct epoll_event> result;
            result.resize(std::thread::hardware_concurrency());
            return result;
        }
        enum class Status:int{
            READY,
            SUSPENDED,
            INACTIVE          
        };
    }
    enum class Transaction{
        ACCEPT,
        DECLINE,
        CANCEL
    };

    enum class Familly : uint8_t{
        UNDEF = AF_UNSPEC,
        IPv4 = AF_INET,
        IPv6 = AF_INET6
    };

    enum class Protocol{
            IP = 0,	   /* Dummy protocol for TCP.  */
            ICMP = 1,	   /* Internet Control Message Protocol.  */
            IGMP = 2,	   /* Internet Group Management Protocol. */
            IPIP = 4,	   /* IPIP tunnels (older KA9Q tunnels use 94).  */
            TCP = 6,	   /* Transmission Control Protocol.  */
            EGP = 8,	   /* Exterior Gateway Protocol.  */
            PUP = 12,	   /* PUP protocol.  */
            UDP = 17,	   /* User Datagram Protocol.  */
            IDP = 22,	   /* XNS IDP protocol.  */
            TP = 29,	   /* SO Transport Protocol Class 4.  */
            DCCP = 33,	   /* Datagram Congestion Control Protocol.  */
            IPV6 = 41,     /* IPv6 header.  */
            RSVP = 46,	   /* Reservation Protocol.  */
            GRE = 47,	   /* General Routing Encapsulation.  */
            ESP = 50,      /* encapsulating security payload.  */
            AH = 51,       /* authentication header.  */
            MTP = 92,	   /* Multicast Transport Protocol.  */
            BEETPH = 94,   /* IP option pseudo header for BEET.  */
            ENCAP = 98,	   /* Encapsulation Header.  */
            PIM = 103,	   /* Protocol Independent Multicast.  */
            COMP = 108,	   /* Compression Header Protocol.  */
            SCTP = 132,	   /* Stream Control Transmission Protocol.  */
            UDPLITE = 136, /* UDP-Lite protocol.  */
            MPLS = 137,    /* MPLS in IP.  */
            ETHERNET = 143, /* Ethernet-within-IPv6 Encapsulation.  */
            RAW = 255,	   /* Raw IP packets.  */
            MPTCP = 262,   /* Multipath TCP connection.  */
    };

    using Port = uint16_t;
    using FileDescriptor = int;
}

namespace network{
    std::ostream& print_ip_port(std::ostream& stream,const sockaddr_storage& addr);
    std::string ip_to_text(const sockaddr_storage& addr);
    std::string port_to_text(const sockaddr_storage& addr);
    std::string protocol_to_text(const sockaddr_storage& addr);
    bool is_correct_address(const std::string& text) noexcept;
    bool is_correct_address(std::string_view text) noexcept;
}