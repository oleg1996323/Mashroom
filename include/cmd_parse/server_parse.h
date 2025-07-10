#pragma once
#include "network/server.h"
#include "cmd_parse/cmd_translator.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/config.h"
#include "sys/log_err.h"
#include <boost/program_options.hpp>
#include <set>


namespace parse{
    ErrorCode add_server_config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode set_server_config_notifier(const std::string& input) noexcept;
    ErrorCode setup_server_config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode remove_server_config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode launch_notifier(const std::string&) noexcept;
    ErrorCode shutdown_notifier(bool wait) noexcept;
    ErrorCode closing_notifier(bool wait) noexcept;
    ErrorCode get_current_config() noexcept;

    class ServerConfig:public BaseParser<ServerConfig>{
        ServerConfig():BaseParser("Server config use:"){}

        virtual void init() noexcept override final{
            descriptor_.add_options()
                    ("add,A","Adds a new instance of the server configuration.")
                    ("setup","Setups an existing instance of the server configuration.")
                    ("remove,R",po::value<std::vector<std::string>>()->notifier([this](const auto& items){
                        err_ = remove_server_config_notifier(items);
                    }),"Removes existing configurations of the server instance. Couldn't be \"default\".")
                    ("set",po::value<std::string>()->notifier([this](const auto& item){
                        err_ = set_server_config_notifier(item);
                    }),"Sets active an existing instance of the server configuration.")
                    ("get-current,GETC","Launch an existing configuration server instance");
            define_uniques();
        }

        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& tokens) noexcept override final{
            
            if(vm.contains("add"))
                return add_server_config_notifier(tokens);
            else if(vm.contains("setup"))
                return setup_server_config_notifier(tokens);
            else if(vm.contains("get-current")){
                std::cout<<Application::config().get_current_server_config().name_<<std::endl;
                return ErrorCode::NONE;
            }
            else{
                err_ = try_notify(vm);
                if(err_!=ErrorCode::NONE)
                    return err_;
                return ErrorCode::NONE;   
            }
        }
    };

    class ServerAction:public BaseParser<ServerAction>{
        ServerAction():BaseParser("Server use:"){}

        virtual void init() noexcept override final{
            descriptor_.add_options()
                    ("launch,L",po::value<std::string>()->implicit_value("")->notifier(launch_notifier),"Launch a server with named configuration or configuration which was previously set.")
                    ("shutdown,S",po::value<bool>()->default_value(true)->notifier(shutdown_notifier),"Shutdown an instance of the server with set configuration.")
                    ("close",po::value<bool>()->default_value(true)->notifier(closing_notifier),"Close a launched server instance");
            define_uniques();
        }

        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& tokens) noexcept override final{
            err_ = try_notify(vm);
            if(err_!=ErrorCode::NONE)
                return err_;
            return ErrorCode::NONE;
        }
    };

    ErrorCode host_notifier(const std::string& input,network::server::Settings& set) noexcept;
    ErrorCode port_notifier(int input,network::server::Settings& set) noexcept;
    ErrorCode timeout_notifier(int timeout,network::server::Settings& set) noexcept;
    ErrorCode protocol_notifier(const std::string& input,network::server::Settings& set) noexcept;
    ErrorCode add_addresses(const std::vector<std::string>& addrs,network::server::Config& config) noexcept;

    class ServerConfig:public BaseParser<ServerConfig>{    
        ServerConfig():BaseParser("Server config options:"){}
        virtual void init() noexcept override final{
            descriptor_.add_options()
                    ("name,N",po::value<std::string>()->required(),"Name of the server configuration.")
                    ("accepted-addresses,A",po::value<std::vector<std::string>>()->default_value({"255.255.255.255"}),"Set the accepted addresses from which the requests could be received.")
                    ("host,H",po::value<std::string>()->required(),"Sets the current host address.")
                    ("port,P",po::value<int>()->required(),"Sets the used port for server using.")
                    ("timeout,T","Sets the common timeout at transactions.")
                    ("help,h","Show help.");
        }
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