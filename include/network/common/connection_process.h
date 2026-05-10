#pragma once
#include <type_traits>
namespace network{
    class Client;
    class Server;
}
namespace network::connection{
    using client_t = network::Client;
    using server_t = network::Server;
    template<typename T>
    requires std::is_same_v<client_t,T> || std::is_same_v<server_t,T>
    class Process;
}