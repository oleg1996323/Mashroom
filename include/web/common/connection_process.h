#pragma once
#include <type_traits>
namespace network{
    class Client;
    class Server;

    using client_t = Client;
    using server_t = Server;
    class ServerConnectionProcess;
    class ClientConnectionProcess;
}