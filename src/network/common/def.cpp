#include <network/common/def.h>

namespace network{
    std::ostream& print_ip_port(std::ostream& stream,addrinfo* addr){
        char ipstr[INET6_ADDRSTRLEN];
        if(addr->ai_family==AF_INET){
            sockaddr_in* a = (sockaddr_in *)addr->ai_addr;
            stream<<"IPv4="<<inet_ntop(addr->ai_family, &a->sin_addr, ipstr, sizeof(ipstr))<<
            " port="<<ntohs(a->sin_port)<<std::endl;
            return stream;
        }
        else {
            sockaddr_in6* a = (sockaddr_in6 *)addr->ai_addr;
            stream<<"IPv6="<<inet_ntop(addr->ai_family, &a->sin6_addr, ipstr, sizeof(ipstr))<<
            " port="<<ntohs(a->sin6_port)<<std::endl;
            return stream;
        }
    }
}