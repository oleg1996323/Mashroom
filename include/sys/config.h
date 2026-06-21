#pragma once
#include "error_print.h"
#include "error_code.h"

#include "sys/config/network.h"
#include "sys/config/user.h"
#include "sys/config/system.h"
namespace fs = std::filesystem;

#include "types/time_interval.h"

class Config{   
    private:
    sys::Config sys_settings_;
    user::Config user_configs_;
    network::Config network_configs_;
    public:
    Config(){
        load();
    }
    Config& operator=(const Config&) = delete;
    Config& operator=(Config&&) = delete;
    Config(const Config&) = delete;
    Config(Config&&) = delete;
    ~Config(){}
    network::client::Config& client_config() noexcept{
        return network_configs_.client_config();
    }
    network::server::Config& server_config() noexcept{
        return network_configs_.server_config();
    }
    user::Config& user_config() noexcept{
        return user_configs_;
    }
    sys::Config& system_config() noexcept{
        return sys_settings_;
    }
    ErrorCode save() noexcept;
    ErrorCode load() noexcept;
};

//config_file
//1. user config commands
//2. data saved by check_command
//3. LogError info (directory)