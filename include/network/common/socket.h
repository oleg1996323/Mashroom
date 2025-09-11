#pragma once
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <memory>
#include "network/common/def.h"

namespace network{

template<__socket_type SOCK_T>
class Socket{
    std::unique_ptr<sockaddr_storage> storage;
    int socket_=-1;
    public:
    Socket(){
        
    }
    int set_no_block(bool noblock) noexcept{
        if(int flags = fcntl(socket,F_GETFL,0)==-1){
            return flags;
        }
        else{
            if(int err = fcntl(socket,F_SETFL,flags|O_NONBLOCK)==-1)
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
        return getsockopt(socket_,SOL_SOCKET,SO_ERROR,&error,&len)==0;
    }
    int descriptor() const noexcept{
        return socket_;
    }
};
}