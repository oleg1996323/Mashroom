#pragma once
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <memory>
#include <stdexcept>
#include "def.h"

namespace network{

//template<__socket_type SOCK_T>
class Socket{
    enum class Options{
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
        Type = SO_TYPE                      //Socket type.
    };
    enum class Type{
        Datagramm = SOCK_DGRAM,             //Datagram socket.
        Raw = SOCK_RAW,                     //Raw Protocol Interface.
        SequencedPack = SOCK_SEQPACKET,     //Sequenced-packet socket.
        Stream = SOCK_STREAM,               //Byte-stream socket.
        NonBlock = SOCK_NONBLOCK            //Non-block socket type         
    };
    std::unique_ptr<sockaddr_storage> storage;
    int socket_=-1;
    private:
    int __descriptor__() const noexcept{
        return socket_;
    }
    Socket& operator=(const Socket& other){
        if(other.__descriptor__()==__descriptor__())
            dup(__descriptor__());
        else{
            close(__descriptor__());
            socket_ = other.__descriptor__();
        }
        return *this;
    }
    Socket& operator=(Socket&& other) noexcept{
        if(other.__descriptor__()!=__descriptor__()){
            close(__descriptor__());
            socket_ = other.__descriptor__();
        }
        return *this;
    }
    Socket(const Socket& other){
        *this = other;
    }
    public:
    Socket() = default;
    Socket(Familly fam,Type type,Protocol proto){
        if(socket_ = socket(static_cast<int>(fam),static_cast<int>(type),static_cast<int>(proto));socket_==-1)
            switch(errno){
                case EAFNOSUPPORT:
                case EINVAL:
                case EMFILE:
                case EPROTONOSUPPORT:
                    throw std::invalid_argument(strerror(errno));
                case ENOBUFS:
                case ENOMEM:
                case EACCES:
                    throw std::runtime_error(strerror(errno));
            }
    }
    ~Socket(){
        close(socket_);
    }
    int bind(const std::unique_ptr<sockaddr_storage>& socket_addr) const{
        if(::bind(socket_,(sockaddr*)&socket_addr,socklen)==-1){
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
                break;
            }
            ::close(socket_);
            return;
        }
    }
    int set_no_block(bool noblock) noexcept{
        if(int flags = fcntl(socket_,F_GETFL,0)==-1){
            return flags;
        }
        else{
            if(int err = fcntl(socket_,F_SETFL,flags|O_NONBLOCK)==-1)
                return err;
        }
        return 0;
    }
    bool is_non_block() const noexcept{
        int error = 0;
        socklen_t len = sizeof(error);
        if(socket_>=0){
            int ret = getsockopt(socket_,SOL_SOCKET,O_NONBLOCK,&error,&len);
            if(ret!=0 || error!=0)
                return false;
        }
        return true;
    }

    bool is_connected() const{
        int error = 0;
        socklen_t len = sizeof(error);
        if(socket_>=0){
            int ret = getsockopt(socket_,SOL_SOCKET,SO_ERROR,&error,&len);
            if(ret!=0 || error!=0)
                return false;
        }
        return true;
    }
    bool is_valid() const noexcept{
        int error = 0;
        socklen_t len = sizeof(error);
        return (getsockopt(socket_,SOL_SOCKET,SO_ERROR,&error,&len)==0 && error==0);
    }
};
}