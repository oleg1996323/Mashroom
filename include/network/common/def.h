#pragma once
#include <sys/error_code.h>
#include <sys/error_print.h>
#include <code_tables/table_0.h>
#include <code_tables/table_4.h>
#include <code_tables/table_5.h>
#include <code_tables/table_6.h>

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
#include "float_conv.h"



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
    enum class Transaction{
        ACCEPT,
        DECLINE,
        CANCEL
    };

    using Socket = int;
    using Port = short;
}

namespace network{
    std::ostream& print_ip_port(std::ostream& stream,addrinfo* addr);
    std::string ip_to_text(addrinfo* addr);
    std::string port_to_text(addrinfo* addr);
    bool is_correct_address(const std::string& text) noexcept;
    bool is_correct_address(std::string_view text) noexcept;
}