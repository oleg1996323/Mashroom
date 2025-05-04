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

using Socket = int;
using Port = short;

namespace server{
    enum class Status{
        READY,
        SUSPENDED,
        INACTIVE            
    };
}

constexpr const char* config_arg_type_conn[]={
    "-auto",
    "-stream",
    "-dgramm",
    "-raw",
    "-rdm",
    "-seq",
    "-dccp"
};

// Тип сокета
enum class SocketType : uint8_t {
    client_socket = 0,
    server_socket = 1
};