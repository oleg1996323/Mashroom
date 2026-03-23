#pragma once
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <memory>
#include <stdexcept>
#include "definitions.h"
#include <cstring>
#include <sys/un.h>
#include <cassert>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <type_traits>
#include <atomic>

namespace network{

enum class SEND_FLAGS;
enum class RECV_FLAGS;

class CommonServer;
class Multiplexor;

namespace detail {
struct Socket{
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
    template<typename T>
    struct Option{
        T value_;
        Options opt_;
        Option() = default;
        Option(const T& value,
            Options option):
            value_(value),opt_(option){}
        Option(T&& value,
            Options option) noexcept:
            value_(std::move(value)),opt_(option){}
    };
};
class SocketControlBlock final{
    friend class SocketHandle;
    friend std::hash<SocketControlBlock>;
    friend std::equal_to<SocketControlBlock>;
    sockaddr_storage storage_;
    int socket_ = -1;
    std::atomic<uint32_t> refcount_{1};
    std::atomic<bool> busy_ = false;
    std::atomic<bool> closing_ = false;
    bool in_epoll = false;
    
    private:
    SocketControlBlock(int raw_socket_id,const sockaddr_storage&);
    public:
    SocketControlBlock& operator=(SocketControlBlock&& other) noexcept = delete;
    SocketControlBlock& operator=(const SocketControlBlock& other) = delete;
    SocketControlBlock(const SocketControlBlock& other) = delete;
    SocketControlBlock(SocketControlBlock&& other) = delete;
    SocketControlBlock(int fd):socket_(fd){}
    ~SocketControlBlock(){
        if(socket>=0){
            ::close(socket_);
            socket_ = -1;
        }
    }
};

class SocketHandle {
private:
    friend class CommonServer;
    friend class CommonClient;
    friend int receive(
            const SocketControlBlock& socket,
            auto&& buffer,
            uint64_t n,
            RECV_FLAGS flags) noexcept 
        requires(
        (std::ranges::view<std::decay_t<decltype(buffer)>> && 
        std::ranges::random_access_range<std::decay_t<decltype(buffer)>> &&
        std::is_rvalue_reference_v<decltype(buffer)>) || 
        (std::ranges::random_access_range<std::decay_t<decltype(buffer)>> &&
        std::is_lvalue_reference_v<decltype(buffer)>));
    friend int send(const SocketControlBlock& socket,SEND_FLAGS flags,
        const std::ranges::random_access_range auto&... buffers) noexcept 
        requires((
        sizeof(decltype(buffers))+...)>0);
    friend class FileSender;
    
    detail::SocketControlBlock* ctrl_ = nullptr;
    int __fd__() const noexcept {
        return ctrl_->socket_;
    }
    sockaddr_storage __init_storage__(
            std::ranges::common_range auto&& host,
            Port port)
    {
        sockaddr_storage tmp;
        if(auto sock4 = reinterpret_cast<sockaddr_in*>(&tmp);
            inet_pton(AF_INET,host.c_str(),&sock4->sin_addr)==1){
            sock4->sin_family = AF_INET;
            sock4->sin_port = htons(port);
        }
        else{
            if(auto* sock6 = reinterpret_cast<sockaddr_in6*>(&tmp);
            inet_pton(AF_INET6,host.c_str(),&sock6->sin6_addr)==1){
                sock6->sin6_family = AF_INET6;
                sock6->sin6_port = htons(port);
            }
            else{
                using namespace std::string_literals;
                throw std::invalid_argument("Invalid host: "s+host.c_str());
            }
        }
        return tmp;
    }
    void __retain__() noexcept {
        if (ctrl_)
            ctrl_->refcount_.fetch_add(1,
                std::memory_order_acq_rel);
    }

    void __release__() noexcept {
        if(ctrl_ && ctrl_->refcount_.fetch_sub(1,
                std::memory_order_acq_rel)==1)
            delete ctrl_;
        ctrl_ = nullptr;
    }
public:
    SocketHandle(SocketControlBlock& socket):
        ctrl_(&socket){}
    SocketHandle(const SocketHandle& other) noexcept
        : ctrl_(other.ctrl_)
    {
        __retain__();
    }
    SocketHandle(SocketHandle&& other) noexcept
        :ctrl_(std::exchange(other.ctrl_, nullptr)){}
    SocketHandle& operator=(const SocketHandle& other) noexcept {
        if (this == &other)
            return *this;
        if (other.ctrl_)
            other.ctrl_->refcount_.fetch_add(1,
                std::memory_order_acq_rel);
        __release__();
        ctrl_ = other.ctrl_;
        return *this;
    }
    SocketHandle& operator=(SocketHandle&& other) noexcept {
        if (this == &other)
            return *this;
        __release__();
        ctrl_ = std::exchange(other.ctrl_, nullptr);
        return *this;
    }
    ~SocketHandle(){
        __release__();
    }
    friend void swap(SocketHandle& lhs,SocketHandle& rhs) noexcept{
        std::swap(lhs.ctrl_,rhs.ctrl_);
    }
    SocketHandle& bind(){
        socklen_t socklen = address_struct_size(ctrl_->storage_);
        if(::bind(ctrl_->socket_,
            reinterpret_cast<sockaddr*>(&ctrl_->storage_),
            socklen)==-1){
            switch (errno)
            {
                case EACCES:
                case EBADF:
                case ENOTSOCK:
                case ELOOP:
                case EROFS:
                    ::close(ctrl_->socket_);
                    break;
                case EADDRINUSE:
                    throw std::runtime_error(strerror(errno));
                case EINVAL:
                case EADDRNOTAVAIL:
                case EFAULT:
                case ENAMETOOLONG:
                case ENOENT:
                case ENOMEM:
                    ::close(ctrl_->socket_);
                    throw std::invalid_argument(strerror(errno));
                    break;
                default:
                    throw std::runtime_error(strerror(errno));
                    break;
            }
        }
        return *this;
    }
    SocketHandle(const std::string& host, Port port, Socket::Type type,Protocol proto){
        auto storage = __init_storage__(host,port);
        switch (static_cast<Family>(storage.ss_family))
        {
        case Family::IPv4:{
            auto sock4 = reinterpret_cast<sockaddr_in*>(
                    &storage);
            sock4->sin_family = AF_INET;
            sock4->sin_port = htons(port);
            break;
        }
        case Family::IPv6:{
            auto sock6 = reinterpret_cast<sockaddr_in6*>(&storage);
            sock6->sin6_family = AF_INET6;
            sock6->sin6_port = htons(port);
        }
        default:
            throw std::invalid_argument(strerror(EAFNOSUPPORT));
            break;
        }
        if(int raw_socket = socket(storage.ss_family,
                static_cast<int>(type),
                static_cast<int>(proto));
                raw_socket==-1){
            switch(errno){
                case EAFNOSUPPORT:
                case EINVAL:
                case EMFILE:
                case EPROTONOSUPPORT:{
                    throw std::invalid_argument(strerror(errno));
                }
                case ENOBUFS:
                case ENOMEM:
                case EACCES:
                    throw std::runtime_error(strerror(errno));
            }
        }
        else {
            auto* ctrl = new SocketControlBlock(raw_socket, storage);
            ctrl_ = ctrl;
        }
    }
    bool operator==(const SocketHandle& other) const
    {return other.ctrl_==this->ctrl_;}
    bool is_valid() const noexcept {
        return ctrl_ && !ctrl_->closing_.load(std::memory_order_acquire);
    }
    template<typename T>
    SocketHandle& set_option(Socket::Option<T> option){
        if(ctrl_->socket_>=0){
            if(setsockopt(*socket_,SOL_SOCKET,static_cast<int>(option.opt_),
                (const char*)&option.value_,sizeof(T))!=0)
                throw std::runtime_error(strerror(errno));
        }
        else throw std::runtime_error(strerror(ENOTSOCK));
        return *this;
    }
    template<typename... ARGS>
    SocketHandle& set_options(Socket::Option<ARGS>... options){
        if(ctrl_->socket_>=0)
            (set_option(options),...);
        else throw std::runtime_error(strerror(ENOTSOCK));
        return *this;
    }
    const sockaddr_storage& get_address_storage() const{
        return ctrl_->storage_;
    }
    SocketHandle& set_no_block(bool noblock){
        int flags = fcntl(ctrl_->socket_,F_GETFL,0);
        if(flags==-1)
            throw std::runtime_error(strerror(errno));
        else{
            if(noblock){
                if(fcntl(ctrl_->socket_,F_SETFL,flags|O_NONBLOCK)==-1)
                    throw std::runtime_error(strerror(errno));
                else return *this;
            }
            else{
                if(fcntl(ctrl_->socket_,F_SETFL,flags&~O_NONBLOCK)==-1)
                    throw std::runtime_error(strerror(errno));
                else return *this;
            }
        }
        return *this;
    }
    bool is_non_block() const{
        int flags = fcntl(__fd__(), F_GETFL, 0);
        return (flags & O_NONBLOCK) != 0;
    }
    void print_address_info(std::ostream& stream) const{
        print_ip_port(stream,ctrl_->storage_);
    }
    std::string ip_to_text() const{
        return ::network::ip_to_text(ctrl_->storage_);
    }
    std::string port_to_text() const{
        return ::network::port_to_text(ctrl_->storage_);
    }
    void mark_closing() noexcept{
        if(ctrl_)
            ctrl_->closing_.store(true,
                std::memory_order_release);
    }
    void send();
    void shutdown();
    template<typename T>
    SocketHandle operator<<(const T& val);
    template<typename T>
    SocketHandle operator>>(T& val);
    
};
}
}

network::detail::Socket::Options operator|(
        network::detail::Socket::Options lhs,
        network::detail::Socket::Options rhs) noexcept;
network::detail::Socket::Options operator&(
        network::detail::Socket::Options lhs,
        network::detail::Socket::Options rhs) noexcept;
network::detail::Socket::Options operator^(
        network::detail::Socket::Options lhs,
        network::detail::Socket::Options rhs) noexcept;
network::detail::Socket::Options operator~(
        network::detail::Socket::Options val) noexcept;
network::detail::Socket::Options operator&(
        network::detail::Socket::Options lhs,
        int rhs) noexcept;
network::detail::Socket::Options operator&(
        std::underlying_type_t<network::detail::Socket::Options> lhs,
        network::detail::Socket::Options rhs) noexcept;
network::detail::Socket::Options operator|(
        network::detail::Socket::Options lhs,
        std::underlying_type_t<network::detail::Socket::Options> rhs) noexcept;
network::detail::Socket::Options operator|(
        std::underlying_type_t<network::detail::Socket::Options> lhs,
        network::detail::Socket::Options rhs) noexcept;
network::detail::Socket::Type operator|(
        network::detail::Socket::Type lhs,
        network::detail::Socket::Type rhs) noexcept;
network::detail::Socket::Type operator&(
        network::detail::Socket::Type lhs,
        network::detail::Socket::Type rhs) noexcept;
network::detail::Socket::Type operator^(
        network::detail::Socket::Type lhs,
        network::detail::Socket::Type rhs) noexcept;
network::detail::Socket::Type operator~(
        network::detail::Socket::Type val) noexcept;
network::detail::Socket::Type operator&(
        network::detail::Socket::Type lhs,int rhs) noexcept;
network::detail::Socket::Type operator&(
        std::underlying_type_t<network::detail::Socket::Type> lhs,
        network::detail::Socket::Type rhs) noexcept;
network::detail::Socket::Type operator|(
        network::detail::Socket::Type lhs,
        std::underlying_type_t<network::detail::Socket::Type> rhs) noexcept;
network::detail::Socket::Type operator|(
        std::underlying_type_t<network::detail::Socket::Type> lhs,
        network::detail::Socket::Type rhs) noexcept;
template<>
struct std::equal_to<network::detail::SocketControlBlock>{
    using is_transparent = std::true_type;
    bool operator()(const network::detail::SocketControlBlock& lhs,
            const network::detail::SocketControlBlock& rhs) const{
        return lhs.socket_==rhs.socket_;
    }
    bool operator()(const network::detail::SocketControlBlock& socket,
            int raw_socket) const{
        return socket.socket_==raw_socket;
    }
    bool operator()(int raw_socket,
            const network::detail::SocketControlBlock& socket) const{
        return socket.socket_==raw_socket;
    }
};
template<>
struct std::hash<network::detail::SocketControlBlock>{
    using is_transparent = std::true_type;
    size_t operator()(const network::detail::SocketControlBlock& socket) const{
        return socket.socket_;
    }
    size_t operator()(int raw_socket) const{
        return raw_socket;
    }
};