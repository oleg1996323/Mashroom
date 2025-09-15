#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>

#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/sendfile.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <thread>

namespace network{
    enum class Side{
        SERVER,
        CLIENT
    };

    template<Side S>
    constexpr Side sent_from(){
        if constexpr(S == Side::SERVER)
            return Side::CLIENT;
        else return Side::SERVER;
    }

    namespace server{
        enum class Status:int{
            READY,
            SUSPENDED,
            INACTIVE          
        };
    }
    enum class Transaction{
        ACCEPT,
        DECLINE,
        CANCEL
    };
}