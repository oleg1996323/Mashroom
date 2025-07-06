#pragma once
#include <program/mashroom.h>
#include <network/server.h>
#include <cmd_parse/cmd_translator.h>
#include <sys/error_code.h>
#include <sys/error_print.h>
#include <sys/config.h>
#include <sys/log_err.h>
#include <boost/program_options.hpp>
#include <set>


namespace parse{
    namespace po = boost::program_options;
    template<typename IterCheck,typename UniqueIter>
    UniqueIter contains_unique_value(IterCheck check1, IterCheck check2, UniqueIter unique1,UniqueIter unique2){
        bool contains_unique = false;
        for(UniqueIter iter = unique1;iter!=unique2;++iter){
            ptrdiff_t count = std::count(check1,check2,*iter);
            if(count==1)
                if(contains_unique==false)
                    contains_unique=true;
                else return iter;
            else if(count>1)
                return iter;
            else continue;
        }
        return unique2;
    }

    ErrorCode add_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode set_notifier(const std::string& input) noexcept;
    ErrorCode setup_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode remove_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode launch_notifier(const std::string&) noexcept;
    ErrorCode shutdown_notifier(bool wait) noexcept;
    ErrorCode closing_notifier(bool wait) noexcept;
    ErrorCode get_current_config() noexcept;

    class ServerAction:public BaseParser<ServerAction>{
        ServerAction():BaseParser("Server use:"){}

        virtual void __init__() noexcept override final{
            po::options_description server_config_actions{"Server configuration:"};
            po::options_description server_application_set{"Server operation:"};
            server_config_actions.add_options()
                    ("add,A","Adds a new instance of the server configuration.")
                    ("setup","Setups an existing instance of the server configuration.")
                    ("remove,R",po::value<std::vector<std::string>>()->notifier(remove_notifier),"Removes an existing configuration of the server instance. Couldn't be \"default\".")
                    ("set",po::value<std::string>(),"Sets active an existing instance of the server configuration.")
                    ("get-current,GETC","Launch an existing configuration server instance");
            server_application_set.add_options()
                    ("launch,L",po::value<std::string>()->implicit_value("")->notifier(launch_notifier),"Launch a server with named configuration or configuration which was previously set.")
                    ("shutdown,S",po::value<bool>()->default_value(true)->notifier(shutdown_notifier),"Shutdown an instance of the server with set configuration.")
                    ("close",po::value<bool>()->default_value(true)->notifier(closing_notifier),"Close a launched server instance");

            descriptor_.add(server_config_actions).add(server_application_set);
            define_uniques();
        }

        virtual ErrorCode __parse__(const std::vector<std::string>& args) noexcept override final{
            po::variables_map vm;
            ErrorCode err_ = ErrorCode::NONE;
            auto parse_result = try_parse(instance().descriptor(),args);
            if(!parse_result.has_value())
                return parse_result.error();
            po::variables_map vm;
            po::store(parse_result.value(),vm);
            std::vector<std::string> tokens = po::collect_unrecognized(parse_result.value().options,po::collect_unrecognized_mode::include_positional);
            auto dublicate = contains_unique_value(tokens.begin(),tokens.end(),unique_values_.begin(),unique_values_.end());
            if(dublicate!=unique_values_.end())
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"must be unique",AT_ERROR_ACTION::CONTINUE,*dublicate);
            if(vm.contains("add"))
                return add_notifier(tokens);
            else if(vm.contains("setup"))
                return setup_notifier(tokens);
            else{
                err_ = try_notify(vm);
                if(err_!=ErrorCode::NONE)
                    return err_;
                return ErrorCode::NONE;   
            }
        }

        public:
        virtual ErrorCode parse(const std::vector<std::string>& args) noexcept override final{
            return __parse__(args);
        }
    };

    ErrorCode host_notifier(const std::string& input,network::server::Settings& set) noexcept;
    ErrorCode port_notifier(int input,network::server::Settings& set) noexcept;
    ErrorCode timeout_notifier(int timeout,network::server::Settings& set) noexcept;
    ErrorCode protocol_notifier(const std::string& input,network::server::Settings& set) noexcept;
    ErrorCode add_addresses(const std::vector<std::string>& addrs,network::server::Config& config) noexcept;

    class ServerConfig:public BaseParser<ServerConfig>{    
        ServerConfig():BaseParser("Server config options:"){}
        virtual void __init__() noexcept override final{
            descriptor_.add_options()
                    ("name,N",po::value<std::string>()->required(),"Name of the server configuration.")
                    ("accepted-addresses,A",po::value<std::vector<std::string>>()->default_value({"255.255.255.255"}),"Set the accepted addresses from which the requests could be received.")
                    ("host,H",po::value<std::string>()->required(),"Sets the current host address.")
                    ("port,P",po::value<int>()->required(),"Sets the used port for server using.")
                    ("timeout,T","Sets the common timeout at transactions.")
                    ("help,h","Show help.");
        }
        virtual ErrorCode __parse__(const std::vector<std::string>& args) noexcept override final;
        public:
        static ServerConfig& instance() noexcept{
            static ServerConfig inst;
            return inst;
        }

        static std::unique_ptr<network::server::Config>& config() noexcept{
            static std::unique_ptr<network::server::Config> config;
            return config;
        }

        virtual ErrorCode parse(const std::vector<std::string>& args) noexcept override final{
            config() = std::make_unique<network::server::Config>();
            ErrorCode err_ = __parse__(args);
            if(err_==ErrorCode::NONE)
                return err_;
            else config().reset();
            return err_;
        }
    };
}