#pragma once
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <memory>
#include <stdexcept>
#include "def.h"
#include <cstring>
#include <sys/un.h>
#include <cassert>

namespace network{

class AbstractServer;
class Multiplexor;

class Socket{
    public:
    enum Options{
        AcceptConnections = SO_ACCEPTCONN,  //Socket is accepting connections.
        BroadCast = SO_BROADCAST,           //Transmission of broadcast messages is supported.
        Debug = SO_DEBUG,                   //Debugging information is being recorded.
        DontRoute = SO_DONTROUTE,           //Bypass normal routing.
        ErrorState = SO_ERROR,              //Socket error status.
        KeepAlive = SO_KEEPALIVE,           //Connections are kept alive with periodic messages.
        Lingers = SO_LINGER,                //Socket lingers on close.
        OutOfBand = SO_OOBINLINE,           //Out-of-band data is transmitted in line.
        BufferSizeIn = SO_RCVBUF,           //Receive buffer size.
        LowWaterMarkIn = SO_RCVLOWAT,       //Receive ``low water mark''.
        TimeOutIn = SO_RCVTIMEO,            //Receive timeout.
        ReuseAddress = SO_REUSEADDR,        //Reuse of local addresses is supported.
        BufferSizeOut = SO_SNDBUF,          //Send buffer size.
        LowWaterMarkOut = SO_SNDLOWAT,      //Send ``low water mark''.
        TimeOutOut = SO_SNDTIMEO,           //Send timeout.
        SocketType = SO_TYPE                //Socket type.
    };
    enum Type{
        Datagramm = SOCK_DGRAM,             //Datagram socket.
        Raw = SOCK_RAW,                     //Raw Protocol Interface.
        SequencedPack = SOCK_SEQPACKET,     //Sequenced-packet socket.
        Stream = SOCK_STREAM,               //Byte-stream socket.
        NonBlock = SOCK_NONBLOCK            //Non-block socket type         
    };
    private:
    friend AbstractServer;
    friend Multiplexor;
    friend std::hash<Socket>;
    friend std::equal_to<Socket>;
    friend int receive(const Socket& socket,std::ranges::random_access_range auto& buffer, uint64_t n) noexcept;
    template<std::ranges::random_access_range... ARGS>
    friend int send(const Socket& socket,const std::ranges::random_access_range auto&... buffers) noexcept;
    std::shared_ptr<sockaddr_storage> storage;
    int socket_=-1;
    private:
    int __descriptor__() const noexcept{
        return socket_;
    }
    void __init_storage__(const /* std::ranges::common_range auto */std::string& host, Port port){
        std::shared_ptr<sockaddr_storage> tmp = std::make_shared<sockaddr_storage>();
        if(auto sock4 = (sockaddr_in*)storage.get();inet_pton(AF_INET,host.c_str(),&sock4->sin_addr)==1){
            sock4->sin_family = AF_INET;
            sock4->sin_port = htons(port);
        }
        else{
            if(auto* sock6 = (sockaddr_in6*)storage.get();inet_pton(AF_INET6,host.c_str(),&sock6->sin6_addr)==1){
                sock6->sin6_family = AF_INET6;
                sock6->sin6_port = htons(port);
            }
            else{
                using namespace std::string_literals;
                throw std::invalid_argument("Invalid host: "s+host.c_str());
            }
        }
        storage = std::move(tmp);
    }
    Socket(int raw_socket_id,const sockaddr_storage&):
    socket_(raw_socket_id),
    storage(std::make_shared<sockaddr_storage>()){}
    public:
    Socket& operator=(Socket&& other) noexcept{
        if(other.__descriptor__()!=__descriptor__()){
            close(__descriptor__());
            socket_ = other.__descriptor__();
            storage = other.storage;
        }
        return *this;
    }
    Socket& operator=(const Socket& other){
        if(other.__descriptor__()==__descriptor__()){
            dup(__descriptor__());
            storage = other.storage;
        }
        else{
            close(__descriptor__());
            socket_ = other.__descriptor__();
            storage = other.storage;
        }
        return *this;
    }
    Socket(const Socket& other){
        *this = other;
    }
    Socket(Socket&& other){
        *this = std::move(other);
    }
    Socket() = default;
    Socket(const std::string& host, Port port, Type type,Protocol proto){
        __init_storage__(host,port);
        switch (static_cast<Familly>(storage->ss_family))
        {
        case Familly::IPv4:{
            auto sock4 = (sockaddr_in*)storage.get();
            sock4->sin_family = AF_INET;
            sock4->sin_port = htons(port);
            break;
        }
        case Familly::IPv6:{
            auto sock6 = (sockaddr_in6*)storage.get();
            sock6->sin6_family = AF_INET6;
            sock6->sin6_port = htons(port);
        }
        default:
            throw std::invalid_argument(strerror(EAFNOSUPPORT));
            break;
        }
        if(socket_ = socket(storage->ss_family,static_cast<int>(type),static_cast<int>(proto));socket_==-1){
            switch(errno){
                case EAFNOSUPPORT:
                case EINVAL:
                case EMFILE:
                case EPROTONOSUPPORT:{
                    storage.reset();
                    throw std::invalid_argument(strerror(errno));
                }
                case ENOBUFS:
                case ENOMEM:
                case EACCES:
                storage.reset();
                    throw std::runtime_error(strerror(errno));
            }
        }
    }
    ~Socket(){
        close(socket_);
    }
    Socket& bind(){
        if(!storage)
            throw std::runtime_error("Undefined binding info");
        socklen_t socklen = storage->ss_family==AF_INET?sizeof(sockaddr_in):
                            (storage->ss_family==AF_INET6?sizeof(sockaddr_in6):
                            sizeof(sockaddr_storage));
        if(::bind(socket_,(sockaddr*)storage.get(),socklen)==-1){
            ::close(socket_);
            switch (errno)
            {
                case EACCES:
                case EADDRINUSE:
                case EBADF:
                case ENOTSOCK:
                case ELOOP:
                case EROFS:
                    throw std::runtime_error(strerror(errno));
                case EINVAL:
                case EADDRNOTAVAIL:
                case EFAULT:
                case ENAMETOOLONG:
                case ENOENT:
                case ENOMEM:
                    throw std::invalid_argument(strerror(errno));
                    break;
                default:
                    throw std::runtime_error(strerror(errno));
                    break;
            }
        }
        return *this;
    }
    template<typename T>
    Socket& set_option(Options opt,T&& val){
        if(socket_>=0){
            if(setsockopt(socket_,SOL_SOCKET,Options::BufferSizeIn,&val,sizeof(T))!=0)
                throw std::runtime_error(strerror(errno));
        }
        else throw std::runtime_error(strerror(ENOTSOCK));
    }
    Socket& set_no_block(bool noblock){
        if(int flags = fcntl(socket_,F_GETFL,0);flags==-1 ||
            fcntl(socket_,F_SETFL,flags|O_NONBLOCK)==-1)
            throw std::runtime_error(strerror(errno));
        return *this;
    }
    bool is_non_block() const{
        if(socket_>=0){
            int flags = 0;
            if(flags = fcntl(socket_,F_GETFL,0);flags==-1 ||
                fcntl(socket_,F_SETFL,flags|O_NONBLOCK)==-1){
                throw std::runtime_error(strerror(errno));
            }
            return flags&O_NONBLOCK;
        }
        else throw std::runtime_error(strerror(ENOTSOCK));
    }
    bool is_connected() const{
        int error = 0;
        socklen_t len = sizeof(error);
        if(socket_>=0){
            if(getsockopt(socket_,SOL_SOCKET,SO_ERROR,&error,&len)!=0)
                throw std::runtime_error(strerror(errno));
            return error==0;
        }
        else throw std::runtime_error(strerror(ENOTSOCK));
    }
    bool is_valid() const{
        int error = 0;
        socklen_t len = sizeof(error);
        if(socket_>=0){
            if(getsockopt(socket_,SOL_SOCKET,SO_ERROR,&error,&len)!=0)
                throw std::runtime_error(strerror(errno));
            return error==0;
        }
        else throw std::runtime_error(strerror(ENOTSOCK));
    }
    void print_address_info(std::ostream& stream){
        if(storage)
            print_ip_port(stream,*storage.get());
        else stream<<"Undefined host:port"<<std::endl;
    }
};
}

network::Socket::Options operator|(network::Socket::Options lhs,network::Socket::Options rhs) noexcept{
    return static_cast<network::Socket::Options>(static_cast<std::underlying_type_t<network::Socket::Options>>(lhs)|
            static_cast<std::underlying_type_t<network::Socket::Options>>(rhs));
}
network::Socket::Options operator&(network::Socket::Options lhs,network::Socket::Options rhs) noexcept{
    return static_cast<network::Socket::Options>(static_cast<std::underlying_type_t<network::Socket::Options>>(lhs)&
            static_cast<std::underlying_type_t<network::Socket::Options>>(rhs));
}
network::Socket::Options operator^(network::Socket::Options lhs,network::Socket::Options rhs) noexcept{
    return static_cast<network::Socket::Options>(static_cast<std::underlying_type_t<network::Socket::Options>>(lhs)^
            static_cast<std::underlying_type_t<network::Socket::Options>>(rhs));
}
network::Socket::Options operator~(network::Socket::Options val) noexcept{
    return static_cast<network::Socket::Options>(~static_cast<std::underlying_type_t<network::Socket::Options>>(val));
}
network::Socket::Options operator&(network::Socket::Options lhs,int rhs) noexcept{
    return static_cast<network::Socket::Options>(static_cast<std::underlying_type_t<network::Socket::Options>>(lhs)&
            rhs);
}
network::Socket::Options operator&(std::underlying_type_t<network::Socket::Options> lhs, network::Socket::Options rhs) noexcept{
    return static_cast<network::Socket::Options>(lhs &
            static_cast<std::underlying_type_t<network::Socket::Options>>(rhs));
}
network::Socket::Options operator|(network::Socket::Options lhs,std::underlying_type_t<network::Socket::Options> rhs) noexcept{
    return static_cast<network::Socket::Options>(static_cast<std::underlying_type_t<network::Socket::Options>>(lhs)|
            rhs);
}
network::Socket::Options operator|(std::underlying_type_t<network::Socket::Options> lhs,network::Socket::Options rhs) noexcept{
    return static_cast<network::Socket::Options>(lhs |
            static_cast<std::underlying_type_t<network::Socket::Options>>(rhs));
}

network::Socket::Type operator|(network::Socket::Type lhs,network::Socket::Type rhs) noexcept{
    return static_cast<network::Socket::Type>(static_cast<std::underlying_type_t<network::Socket::Type>>(lhs)|
            static_cast<std::underlying_type_t<network::Socket::Type>>(rhs));
}
network::Socket::Type operator&(network::Socket::Type lhs,network::Socket::Type rhs) noexcept{
    return static_cast<network::Socket::Type>(static_cast<std::underlying_type_t<network::Socket::Type>>(lhs)&
            static_cast<std::underlying_type_t<network::Socket::Type>>(rhs));
}
network::Socket::Type operator^(network::Socket::Type lhs,network::Socket::Type rhs) noexcept{
    return static_cast<network::Socket::Type>(static_cast<std::underlying_type_t<network::Socket::Type>>(lhs)^
            static_cast<std::underlying_type_t<network::Socket::Type>>(rhs));
}
network::Socket::Type operator~(network::Socket::Type val) noexcept{
    return static_cast<network::Socket::Type>(~static_cast<std::underlying_type_t<network::Socket::Type>>(val));
}
network::Socket::Type operator&(network::Socket::Type lhs,int rhs) noexcept{
    return static_cast<network::Socket::Type>(static_cast<std::underlying_type_t<network::Socket::Type>>(lhs)&
            rhs);
}
network::Socket::Type operator&(std::underlying_type_t<network::Socket::Type> lhs, network::Socket::Type rhs) noexcept{
    return static_cast<network::Socket::Type>(lhs &
            static_cast<std::underlying_type_t<network::Socket::Type>>(rhs));
}
network::Socket::Type operator|(network::Socket::Type lhs,std::underlying_type_t<network::Socket::Type> rhs) noexcept{
    return static_cast<network::Socket::Type>(static_cast<std::underlying_type_t<network::Socket::Type>>(lhs)|
            rhs);
}
network::Socket::Type operator|(std::underlying_type_t<network::Socket::Type> lhs,network::Socket::Type rhs) noexcept{
    return static_cast<network::Socket::Type>(lhs |
            static_cast<std::underlying_type_t<network::Socket::Type>>(rhs));
}
template<>
struct std::equal_to<network::Socket>{
    using is_transparent = std::true_type;
    bool operator()(const network::Socket& lhs,const network::Socket& rhs) const{
        return lhs.__descriptor__()==rhs.__descriptor__();
    }
};
template<>
struct std::hash<network::Socket>{
    using is_transparent = std::true_type;
    size_t operator()(const network::Socket& socket) const{
        return socket.__descriptor__();
    }
};