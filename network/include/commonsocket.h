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

namespace network{

enum class SEND_FLAGS;
enum class RECV_FLAGS;

template<typename DERIVED_CONNECTIONPOOL>
class CommonServer;
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
    template<typename T>
    struct Option{
        T value_;
        Options opt_;
        Option() = default;
        Option(const T& value,Options option):value_(value),opt_(option){}
        Option(T&& value, Options option) noexcept:value_(std::move(value)),opt_(option){}
    };
    private:
    template<typename DERIVED_CONNECTIONPOOL>
    friend class CommonServer;
    template<typename PROCESS_T>
    friend class CommonClient;
    friend Multiplexor;
    friend std::hash<Socket>;
    friend std::equal_to<Socket>;
    friend int receive(const Socket& socket,auto&& buffer, uint64_t n,RECV_FLAGS flags) noexcept requires(
        (std::ranges::view<std::decay_t<decltype(buffer)>> && 
        std::ranges::random_access_range<std::decay_t<decltype(buffer)>> &&
        std::is_rvalue_reference_v<decltype(buffer)>) || 
        (std::ranges::random_access_range<std::decay_t<decltype(buffer)>> &&
        std::is_lvalue_reference_v<decltype(buffer)>));
    friend int send(const Socket& socket,SEND_FLAGS flags,const std::ranges::random_access_range auto&... buffers) noexcept requires((
        sizeof(decltype(buffers))+...)>0);
    std::shared_ptr<sockaddr_storage> storage;
    std::shared_ptr<int> socket_;
    private:
    const std::shared_ptr<sockaddr_storage>& get_address_storage() const{
        return storage;
    }
    int __descriptor__() const noexcept;
    void __init_storage__(std::ranges::common_range auto&& host, Port port);
    Socket(int raw_socket_id,const sockaddr_storage&);
    public:
    Socket& operator=(Socket&& other) noexcept;
    Socket& operator=(const Socket& other);
    bool operator==(const Socket& other) const{return socket_ && other.socket_ && *socket_==*other.socket_;}
    Socket(const Socket& other);
    Socket(Socket&& other);
    Socket() = default;
    Socket(const std::string& host, Port port, Type type,Protocol proto);
    virtual ~Socket();
    Socket& bind();
    template<typename T>
    Socket& set_option(Option<T> option){
        if(__descriptor__()>=0){
            if(setsockopt(*socket_,SOL_SOCKET,static_cast<int>(option.opt_),(const char*)&option.value_,sizeof(T))!=0)
                throw std::runtime_error(strerror(errno));
        }
        else throw std::runtime_error(strerror(ENOTSOCK));
        return *this;
    }
    template<typename... ARGS>
    Socket& set_options(Option<ARGS>... options){
        if(__descriptor__()>=0)
            (set_option(options),...);
        else throw std::runtime_error(strerror(ENOTSOCK));
        return *this;
    }
    Socket& set_no_block(bool noblock);
    bool is_non_block() const;
    //bool is_connected() const;
    bool is_valid() const;
    void print_address_info(std::ostream& stream) const;
};
}

network::Socket::Options operator|(network::Socket::Options lhs,network::Socket::Options rhs) noexcept;
network::Socket::Options operator&(network::Socket::Options lhs,network::Socket::Options rhs) noexcept;
network::Socket::Options operator^(network::Socket::Options lhs,network::Socket::Options rhs) noexcept;
network::Socket::Options operator~(network::Socket::Options val) noexcept;
network::Socket::Options operator&(network::Socket::Options lhs,int rhs) noexcept;
network::Socket::Options operator&(std::underlying_type_t<network::Socket::Options> lhs, network::Socket::Options rhs) noexcept;
network::Socket::Options operator|(network::Socket::Options lhs,std::underlying_type_t<network::Socket::Options> rhs) noexcept;
network::Socket::Options operator|(std::underlying_type_t<network::Socket::Options> lhs,network::Socket::Options rhs) noexcept;
network::Socket::Type operator|(network::Socket::Type lhs,network::Socket::Type rhs) noexcept;
network::Socket::Type operator&(network::Socket::Type lhs,network::Socket::Type rhs) noexcept;
network::Socket::Type operator^(network::Socket::Type lhs,network::Socket::Type rhs) noexcept;
network::Socket::Type operator~(network::Socket::Type val) noexcept;
network::Socket::Type operator&(network::Socket::Type lhs,int rhs) noexcept;
network::Socket::Type operator&(std::underlying_type_t<network::Socket::Type> lhs, network::Socket::Type rhs) noexcept;
network::Socket::Type operator|(network::Socket::Type lhs,std::underlying_type_t<network::Socket::Type> rhs) noexcept;
network::Socket::Type operator|(std::underlying_type_t<network::Socket::Type> lhs,network::Socket::Type rhs) noexcept;
template<>
struct std::equal_to<network::Socket>{
    using is_transparent = std::true_type;
    bool operator()(const network::Socket& lhs,const network::Socket& rhs) const{
        return lhs.__descriptor__()==rhs.__descriptor__();
    }
    bool operator()(const network::Socket& socket,int raw_socket) const{
        return socket.__descriptor__()==raw_socket;
    }
    bool operator()(int raw_socket,const network::Socket& socket) const{
        return socket.__descriptor__()==raw_socket;
    }
};
template<>
struct std::hash<network::Socket>{
    using is_transparent = std::true_type;
    size_t operator()(const network::Socket& socket) const{
        return socket.__descriptor__();
    }
    size_t operator()(int raw_socket) const{
        return raw_socket;
    }
};