#pragma once
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/config.h"
#include "sys/log_err.h"
#include "cmd_parse/cmd_def.h"
#include <boost/program_options.hpp>
#include "cmd_parse/functions.h"

namespace parse{
    ErrorCode add_server_config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode set_server_config_notifier(const std::string& input) noexcept;
    ErrorCode setup_server_config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode remove_server_config_notifier(const std::vector<std::string>& input) noexcept;

    ErrorCode host_notifier(const std::string& input) noexcept;
    ErrorCode port_notifier(int input) noexcept;
    ErrorCode timeout_notifier(int timeout) noexcept;
    ErrorCode protocol_notifier(const std::string& input) noexcept;
    ErrorCode add_addresses(const std::vector<std::string>& addrs) noexcept;

    class ServerConfigOptions:public AbstractCLIParser<ServerConfigOptions>{
        friend AbstractCLIParser;
        ServerConfigOptions():AbstractCLIParser("Server config options"){}

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

    class ServerConfig:public AbstractCLIParser<ServerConfig>{    
        friend AbstractCLIParser;
        ServerConfig():AbstractCLIParser("Server configuration"){}
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
    };
}