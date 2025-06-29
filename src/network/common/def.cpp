#include <network/common/def.h>
#include <expected>
#include <charconv>

using namespace std::string_literals;
namespace network{
    std::string ip_to_text(addrinfo* addr){
        if(addr->ai_family==AF_INET){
            sockaddr_in* a = (sockaddr_in *)addr->ai_addr;
            char ipstr[INET_ADDRSTRLEN];
            return "IPv4:"s+inet_ntop(addr->ai_family, &a->sin_addr, ipstr, sizeof(ipstr));
        }
        else {
            sockaddr_in6* a = (sockaddr_in6 *)addr->ai_addr;
            char ipstr[INET6_ADDRSTRLEN];
            return "IPv6:"s+inet_ntop(addr->ai_family, &a->sin6_addr, ipstr, sizeof(ipstr));
        }
    }

    std::string port_to_text(addrinfo* addr){
        if(addr->ai_family==AF_INET){
            sockaddr_in* a = (sockaddr_in *)addr->ai_addr;
            return "port:"s+std::to_string(ntohs(a->sin_port));
        }
        else {
            sockaddr_in6* a = (sockaddr_in6 *)addr->ai_addr;
            return "port:"s+std::to_string(ntohs(a->sin6_port));
        }
    }

    std::ostream& print_ip_port(std::ostream& stream,addrinfo* addr){
        stream<<ip_to_text(addr)<<" "<<port_to_text(addr)<<std::endl;
        return stream;
    }

    bool is_correct_address(const std::string& text) noexcept{
        {
            sockaddr_in addr;
            int res = inet_pton(AF_INET,text.c_str(),&addr);
            if(res>0)
                return true;
        }
        {
            sockaddr_in6 addr;
            int res = inet_pton(AF_INET,text.c_str(),&addr);
            if(res>0)
                return true;
        }
        return false;
    }
    bool is_correct_address(std::string_view text) noexcept{
        std::string copy(text);
        {
            sockaddr_in addr;
            int res = inet_pton(AF_INET,copy.c_str(),&addr);
            if(res>0)
                return true;
        }
        {
            sockaddr_in6 addr;
            int res = inet_pton(AF_INET,copy.c_str(),&addr);
            if(res>0)
                return true;
        }
        return false;
    }
}