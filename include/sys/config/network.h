#pragma once
#include "sys/config/web/client.h"
#include "sys/config/web/server.h"

namespace network{
class Config{
    network::server::Config server_configs_;
    network::client::Config client_configs_;
    public:
    network::server::Config& server_config() noexcept{
        return server_configs_;
    }
    network::client::Config& client_config() noexcept{
        return client_configs_;
    }
};
}