#pragma once
#include <types/time_interval.h>
#include <types/coord.h>
#include <sys/error_code.h>
#include <sys/error_print.h>
#include <code_tables/table_0.h>
#include <code_tables/table_4.h>
#include <code_tables/table_5.h>
#include <code_tables/table_6.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <extract.h>
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

using Socket = int;
using Port = short;

namespace network::server{
    constexpr int min_timeout_seconds = 1;
    static std::vector<struct epoll_event> define_epoll_event(){
        std::vector<struct epoll_event> result;
        result.resize(std::thread::hardware_concurrency());
        return result;
    }
    enum class Status:int{
        READY,
        SUSPENDED,
        INACTIVE          
    };
}

namespace network{
    std::ostream& print_ip_port(std::ostream& stream,addrinfo* addr);
}