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
    const network::server::Config& server_config() const noexcept{
        return server_configs_;
    }
    const network::client::Config& client_config() const noexcept{
        return client_configs_;
    }
    void server_config(network::server::Config config) noexcept{
        server_configs_=std::forward<network::server::Config>(config);
    }
    void client_config(network::client::Config config) noexcept{
        client_configs_=std::forward<network::client::Config>(config);
    }
};
}

template<>
boost::json::value to_json(const network::Config& val);

template<>
std::expected<network::Config,std::exception> from_json(const boost::json::value& val);