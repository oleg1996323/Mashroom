#include "server_config_parse.h"
#include "functions.h"
#include "sys/application.h"
#include <boost/program_options.hpp>
#include "program/mashroom.h"

namespace parse{

ErrorCode add_addresses(const std::vector<std::string>& addrs) noexcept{
    for(auto addr : addrs){
        {
            sockaddr_in tmp;
            int res = inet_pton(AF_INET,addr.data(),&tmp);
            if(res>0)
                ServerConfigOptions::config()->accepted_addresses_.insert(std::string(addr));
        }
        {
            sockaddr_in6 tmp;
            int res = inet_pton(AF_INET6,addr.data(),&tmp);
            if(res>0)
                ServerConfigOptions::config()->accepted_addresses_.insert(std::string(addr));
            else return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"invalid address",AT_ERROR_ACTION::CONTINUE,addr);
        }
    }
    return ErrorCode::NONE;
}

ErrorCode add_notifier(const std::vector<std::string>& input) noexcept{
    ErrorCode err = ServerConfig::instance().parse(input);
    if(err!=ErrorCode::NONE){
        ServerConfigOptions::config().reset();
        return err;
    }
    else if(std::string_view name = ServerConfigOptions::config()->name_;
        !Application::config().add_server_config(std::move(*ServerConfigOptions::config().release())))
        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server config already exists",AT_ERROR_ACTION::CONTINUE,name);
    else return ErrorCode::NONE;
}

ErrorCode setup_notifier(const std::vector<std::string>& input) noexcept{
    ErrorCode err = ServerConfig::instance().parse(input);
    if(err!=ErrorCode::NONE){
        ServerConfigOptions::config().reset();
        return err;
    }
    else if(std::string_view name = ServerConfigOptions::config()->name_;
        !Application::config().setup_server_config(std::move(*ServerConfigOptions::config().release())))
        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server config doesn't exists",AT_ERROR_ACTION::CONTINUE,name);
    else return ErrorCode::NONE;
}

ErrorCode set_notifier(const std::string& input) noexcept{
    if(std::string_view name = ServerConfigOptions::config()->name_;
        !Application::config().set_server_config(input))
        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server config name don't exists",AT_ERROR_ACTION::CONTINUE,name);
    return ErrorCode::NONE;
}

ErrorCode remove_notifier(const std::vector<std::string>& input) noexcept{
    if(input.empty())
        return ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"server config name must be indicated",AT_ERROR_ACTION::CONTINUE);
    if(std::count(input.begin(),input.end(),"default")!=0)
        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"removing server config failed (indicated \"default\")",AT_ERROR_ACTION::CONTINUE,input.at(1));
    for(std::string_view conf_name:input){
        if(!Application::config().remove_server_config(input.at(1)))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"removing server config failed (config don't exists)",AT_ERROR_ACTION::CONTINUE,input.at(1));
    }
    return ErrorCode::NONE;
}

ErrorCode host_notifier(const std::string& input) noexcept{   
    if(network::is_correct_address(input)){
        ServerConfigOptions::config()->settings_.host=input;
        return ErrorCode::NONE;
    }
    else return ErrorPrint::print_error(ErrorCode::INVALID_HOST_X1,"",AT_ERROR_ACTION::CONTINUE,input);
}

ErrorCode port_notifier(int input) noexcept{
    if(input<1025 || input>std::numeric_limits<uint16_t>::max())
        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"port definition. Must be bigger than 1024",AT_ERROR_ACTION::CONTINUE,input);
    ServerConfigOptions::config()->settings_.port=input;
    return ErrorCode::NONE;
}

ErrorCode timeout_notifier(int timeout) noexcept{
    if(timeout<0)
        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"timeout definition",AT_ERROR_ACTION::CONTINUE,timeout);
    else ServerConfigOptions::config()->settings_.timeout_seconds_ = timeout;
    return ErrorCode::NONE;
}

ErrorCode protocol_notifier(const std::string& input) noexcept{
    if(!getprotobyname(input.c_str()))
        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"protocol definition",AT_ERROR_ACTION::CONTINUE,input);
    else ServerConfigOptions::config()->settings_.protocol = input;
    return ErrorCode::NONE;
}

ErrorCode ServerConfigOptions::execute(vars& vm,const std::vector<std::string>& args) noexcept{
    config() = std::make_unique<network::server::Config>();
    return try_notify(vm);
}

ErrorCode ServerConfig::execute(vars& vm,const std::vector<std::string>& args) noexcept{
    if(vm.contains("add")){
        err_ = ServerConfigOptions::instance().parse(args);
        if(err_==ErrorCode::NONE)
            app().config().add_server_config(std::move(*ServerConfigOptions::config().release()));
        else return err_;
    }
    else if(vm.contains("setup")){
        err_ = ServerConfigOptions::instance().parse(args);
        if(err_==ErrorCode::NONE)
            app().config().setup_server_config(std::move(*ServerConfigOptions::config().release()));
        else return err_;
    }
    else if(vm.contains("remove"))
        app().config().remove_server_config(vm.at("remove").as<std::string>());
    else if(vm.contains("set"))
        app().config().set_server_config(vm.at("set").as<std::string>());
    else if(vm.contains("current"))
        std::cout<<to_json(app().config().get_current_server_config())<<std::endl;
    else
        for(auto& s_config:app().config().get_server_configs())
            std::cout<<to_json(s_config)<<std::endl;
    return ErrorCode::NONE;
}

void ServerConfigOptions::init() noexcept{
    add_options
            ("name,N",po::value<std::string>()->required()->notifier([this](const std::string& arg){
                if(err_!=ErrorCode::NONE)
                    return;
                if(!arg.empty())
                    config()->name_ = arg;
                else err_=ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,"name");
            }),"Name of the server configuration.")
            ("accepted-addresses,A",po::value<std::vector<std::string>>()->notifier([this](const std::vector<std::string>& args){
                if(err_!=ErrorCode::NONE)
                    return;
                if(!args.empty())
                    err_ = add_addresses(args);
                else err_=ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,"accepted-addresses");
            }),"Set the accepted addresses from which the requests could be received.")
            ("host,H",po::value<std::string>()->required()->notifier([this](const std::string& arg){
                if(err_!=ErrorCode::NONE)
                    return;
                if(!arg.empty())
                    err_ = host_notifier(arg);
                else err_=ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,"host");
            }),"Sets the current host address.")
            ("port,P",po::value<int>()->required()->notifier([this](int arg){
                if(err_==ErrorCode::NONE)
                    err_ = port_notifier(arg);
            }),"Sets the used port for server using.")
            ("timeout,T",po::value<int>()->default_value(20)->notifier([this](int arg){
                if(err_==ErrorCode::NONE)
                    err_ = timeout_notifier(arg);
            }),"Sets the common timeout at transactions.");
            define_uniques();
}

void ServerConfig::init() noexcept{
    add_options_instances("add",po::value<std::vector<std::string>>()->zero_tokens(),"Add new server configuration",ServerConfigOptions::instance())
    ("remove",po::value<std::string>()->required(),"Remove existing server configuration")
    ("setup",po::value<std::vector<std::string>>()->zero_tokens(),"Reconfig an existing server configuration",ServerConfigOptions::instance())
    ("set",po::value<std::string>()->required(),"Setup an existing server configuration")
    ("print-all","Print all accessible server configurations")
    ("current","Print current server configuration");
    define_uniques();
}
}