#pragma once
#include <thread>
#include <cstdint>
#include "boost_functional/json.h"
#include "sys/config/base_config.h"
#include "network/clientsettings.h"

namespace network::client{

    struct Config:public BaseConfig<network::client::Settings>{
        public:
        Config() = default;
        void print(std::ostream&) const override;
    };

    Config default_config();
}

template<>
boost::json::value to_json(const network::client::Config& val);

template<>
std::expected<network::client::Config,std::exception> from_json(const boost::json::value& val);