#include "network/common/def.h"
#include <expected>
#include <charconv>

using namespace std::string_literals;
namespace network{
    std::string ip_to_text(sockaddr_storage* addr){
        if(!addr)
            return "";
        sockaddr_in* sock4 = (sockaddr_in*)addr;
        if(sock4->sin_family==AF_INET){
            char ipstr[INET_ADDRSTRLEN];
            return "IPv4:"s+inet_ntop(sock4->sin_family, &sock4->sin_addr, ipstr, sizeof(ipstr));
        }
        else {
            sockaddr_in6* sock6 = (sockaddr_in6*)addr;
            char ipstr[INET6_ADDRSTRLEN];
            return "IPv6:"s+inet_ntop(sock6->sin6_family, &sock6->sin6_addr, ipstr, sizeof(ipstr));
        }
    }

    std::string port_to_text(sockaddr_storage* addr){
        sockaddr_in* sock4 = (sockaddr_in*)addr;
        if(sock4->sin_family==AF_INET){
            return "port:"s+std::to_string(ntohs(sock4->sin_port));
        }
        else {
            sockaddr_in6* sock6 = (sockaddr_in6 *)addr;
            return "port:"s+std::to_string(ntohs(sock6->sin6_port));
        }
    }

    std::ostream& print_ip_port(std::ostream& stream,sockaddr_storage* addr){
        stream<<ip_to_text(addr)<<" "<<port_to_text(addr)<<std::endl;
        return stream;
    }

    bool is_correct_address(const std::string& text) noexcept{
        {
            sockaddr_in addr;
            if(inet_pton(AF_INET,text.c_str(),&addr)==1)
                return true;
        }
        {
            sockaddr_in6 addr;
            if(inet_pton(AF_INET6,text.c_str(),&addr)==1)
                return true;
        }
        return false;
    }
    bool is_correct_address(std::string_view text) noexcept{
        std::string copy(text);
        {
            sockaddr_in addr;
            if(inet_pton(AF_INET,copy.c_str(),&addr)==1)
                return true;
        }
        {
            sockaddr_in6 addr;
            if(inet_pton(AF_INET6,copy.c_str(),&addr)==1)
                return true;
        }
        return false;
    }
}