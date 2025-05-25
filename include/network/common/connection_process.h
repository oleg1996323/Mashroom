#pragma once
#include <type_traits>
namespace network::client{
    class Client;
}
namespace network::server{
    class Server;
}
namespace network::connection{
    using client_t = network::client::Client;
    using server_t = network::server::Server;
    template<typename T>
    requires std::is_same_v<client_t,T> || std::is_same_v<server_t,T>
    class Process;
}