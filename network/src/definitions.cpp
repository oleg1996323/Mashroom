#include "definitions.h"
#include <expected>
#include <charconv>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std::string_literals;
namespace network{
    std::string ip_to_text(const sockaddr_storage& addr){
        switch(static_cast<Familly>(addr.ss_family)){
            case Familly::IPv4:{
                char ipstr[INET_ADDRSTRLEN];
                return "IPv4:"s+inet_ntop(addr.ss_family,&((sockaddr_in*)&addr)->sin_addr,ipstr,sizeof(ipstr));
                break;
            }
            case Familly::IPv6:{
                char ipstr[INET6_ADDRSTRLEN];
                return "IPv6:"s+inet_ntop(addr.ss_family,&((sockaddr_in6*)&addr)->sin6_addr,ipstr,sizeof(ipstr));
                break;
            }
            default:
                return "NaN"s;
                break;
        }
    }

    std::string port_to_text(const sockaddr_storage& addr){
        switch(static_cast<Familly>(addr.ss_family)){
            case Familly::IPv4:
                return "port:"s+std::to_string(ntohs(((sockaddr_in*)&addr)->sin_port));
                break;
            case Familly::IPv6:
                return "port:"s+std::to_string(ntohs(((sockaddr_in6*)&addr)->sin6_port));
                break;
            default:
                return "NaN"s;
                break;
        }
    }
    std::string protocol_to_text(const sockaddr_storage& addr){
        switch(static_cast<Familly>(addr.ss_family)){
            case Familly::IPv4:{
                return "IPv4"s;
                break;
            }
            case Familly::IPv6:{
                char ipstr[INET6_ADDRSTRLEN];
                return "IPv6"s;
                break;
            }
            default:
                return "NaN"s;
                break;
        }
    }

    std::ostream& print_ip_port(std::ostream& stream,const sockaddr_storage& addr){
        stream<<protocol_to_text(addr)<<":"<<ip_to_text(addr)<<" "<<port_to_text(addr)<<std::endl;
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