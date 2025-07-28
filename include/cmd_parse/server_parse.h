#pragma once
#include "network/server.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/config.h"
#include "sys/log_err.h"
#include "cmd_parse/cmd_def.h"
#include <boost/program_options.hpp>
#include <set>
#include "cmd_parse/functions.h"


namespace parse{
    ErrorCode add_server_config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode set_server_config_notifier(const std::string& input) noexcept;
    ErrorCode setup_server_config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode remove_server_config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode launch_notifier(const std::string&) noexcept;
    ErrorCode shutdown_notifier(bool wait) noexcept;
    ErrorCode closing_notifier(bool wait) noexcept;
    ErrorCode get_current_config() noexcept;

    class ServerAction:public AbstractCLIParser<ServerAction>{
        friend AbstractCLIParser;
        ServerAction():AbstractCLIParser("Server use:"){}

        virtual void init() noexcept override final;

        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& tokens) noexcept override final;
    };

    ErrorCode host_notifier(const std::string& input,network::server::Settings& set) noexcept;
    ErrorCode port_notifier(int input,network::server::Settings& set) noexcept;
    ErrorCode timeout_notifier(int timeout,network::server::Settings& set) noexcept;
    ErrorCode protocol_notifier(const std::string& input,network::server::Settings& set) noexcept;
    ErrorCode add_addresses(const std::vector<std::string>& addrs,network::server::Config& config) noexcept;

    class ServerConfig:public AbstractCLIParser<ServerConfig>{    
        friend AbstractCLIParser;
        ServerConfig():AbstractCLIParser("Server config options:"){}
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        virtual void callback() noexcept override final{
            if(err_!=ErrorCode::NONE)
                config().reset();
        }
        public:

        static std::unique_ptr<network::server::Config>& config() noexcept{
            static std::unique_ptr<network::server::Config> config;
            return config;
        }

    };
}