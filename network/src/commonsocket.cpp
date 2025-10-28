#include "commonsocket.h"
#include "byte_order.h"

namespace network{
    int Socket::__descriptor__() const noexcept{
        if(socket_)
            return *socket_;
        return -1;
    }
    void Socket::__init_storage__(std::ranges::common_range auto&& host, Port port){
        std::shared_ptr<sockaddr_storage> tmp = std::make_shared<sockaddr_storage>();
        if(auto sock4 = (sockaddr_in*)tmp.get();inet_pton(AF_INET,host.c_str(),&sock4->sin_addr)==1){
            sock4->sin_family = AF_INET;
            sock4->sin_port = htons(port);
        }
        else{
            if(auto* sock6 = (sockaddr_in6*)tmp.get();inet_pton(AF_INET6,host.c_str(),&sock6->sin6_addr)==1){
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
    Socket::Socket(int raw_socket_id,const sockaddr_storage& stor):
    socket_(std::make_shared<int>(raw_socket_id)),
    storage(std::make_shared<sockaddr_storage>(stor)){}
    Socket& Socket::operator=(Socket&& other) noexcept{
        if(other.__descriptor__()!=__descriptor__()){
            ::close(__descriptor__());
            socket_ = std::move(other.socket_);
            storage = std::move(other.storage);
        }
        return *this;
    }
    Socket& Socket::operator=(const Socket& other){
        if(other.__descriptor__()==__descriptor__()){
            socket_ = other.socket_;
            storage = other.storage;
        }
        else{
            ::close(__descriptor__());
            socket_ = other.socket_;
            storage = other.storage;
        }
        return *this;
    }
    Socket::Socket(const Socket& other){
        *this = other;
    }
    Socket::Socket(Socket&& other){
        *this = std::move(other);
    }
    Socket::Socket(const std::string& host, Port port, Type type,Protocol proto){
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
        if(int raw_socket = socket(storage->ss_family,static_cast<int>(type),static_cast<int>(proto));raw_socket==-1){
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
        else socket_ = std::make_shared<int>(raw_socket);
    }
    Socket::~Socket(){
        if(socket_ && socket_.use_count()<=1){
            std::cout<<"Closing connection: fd="<<*socket_<<" "<<std::endl;
            print_address_info(std::cout);
            ::close(__descriptor__());
        }
    }
    Socket& Socket::bind(){
        if(!storage)
            throw std::runtime_error("Undefined binding info");
        socklen_t socklen = address_struct_size(*storage);
        if(::bind(__descriptor__(),(sockaddr*)storage.get(),socklen)==-1){
            switch (errno)
            {
                case EACCES:
                case EBADF:
                case ENOTSOCK:
                case ELOOP:
                case EROFS:
                    ::close(__descriptor__());
                case EADDRINUSE:
                    throw std::runtime_error(strerror(errno));
                case EINVAL:
                case EADDRNOTAVAIL:
                case EFAULT:
                case ENAMETOOLONG:
                case ENOENT:
                case ENOMEM:
                    ::close(__descriptor__());
                    throw std::invalid_argument(strerror(errno));
                    break;
                default:
                    throw std::runtime_error(strerror(errno));
                    break;
            }
        }
        return *this;
    }
    Socket& Socket::set_no_block(bool noblock){
        int flags = fcntl(__descriptor__(),F_GETFL,0);
        if(flags==-1)
            throw std::runtime_error(strerror(errno));
        else{
            if(noblock){
                if(fcntl(__descriptor__(),F_SETFL,flags|O_NONBLOCK)==-1)
                    throw std::runtime_error(strerror(errno));
                else return *this;
            }
            else{
                if(fcntl(__descriptor__(),F_SETFL,flags&~O_NONBLOCK)==-1)\
                    throw std::runtime_error(strerror(errno));
                else return *this;
            }
        }
        return *this;
    }
    bool Socket::is_non_block() const{
        if(__descriptor__()>=0){
            int flags = 0;
            if(flags = fcntl(__descriptor__(),F_GETFL,0);flags==-1 ||
                fcntl(__descriptor__(),F_SETFL,flags|O_NONBLOCK)==-1)
            {
                #ifdef DEBUG
                    std::cout<<strerror(errno)<<std::endl;
                #endif
                return false;
            }
            return (flags&O_NONBLOCK)!=0;
        }
        else return false;
    }
    // bool Socket::is_connected() const{
    //     int error = 0;
    //     socklen_t len = sizeof(error);
    //     if(__descriptor__()>=0){
    //         if(getsockopt(__descriptor__(),SOL_SOCKET,SO_ERROR,&error,&len)!=0){
    //             #ifdef DEBUG
    //             std::cout<<strerror(errno)<<std::endl;
    //             #endif
    //             return false;
    //         }
    //         return error==0;
    //     }
    //     else return false;
    // }
    bool Socket::is_valid() const{
        int error = 0;
        socklen_t len = sizeof(error);
        if(__descriptor__()>=0){
            if(getsockopt(__descriptor__(),SOL_SOCKET,SO_ERROR,&error,&len)!=0){
                #ifdef DEBUG
                std::cout<<strerror(errno)<<std::endl;
                #endif
                return false;
            }
            return error==0;
        }
        else return false;
    }
    void Socket::print_address_info(std::ostream& stream) const{
        if(storage)
            print_ip_port(stream,*storage.get());
        else stream<<"Undefined host:port"<<std::endl;
    }
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