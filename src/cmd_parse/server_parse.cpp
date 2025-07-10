#include "cmd_parse/server_parse.h"
#include "functions.h"
#include "sys/application.h"
#include <boost/program_options.hpp>
#include "program/mashroom.h"


using namespace translate::token;

namespace parse{
    ErrorCode add_addresses(const std::vector<std::string>& addrs,network::server::Config& config) noexcept{
        for(auto addr : addrs){
            {
                sockaddr_in tmp;
                int res = inet_pton(AF_INET,addr.data(),&tmp);
                if(res>0)
                    config.accepted_addresses_.insert(std::string(addr));
            }
            {
                sockaddr_in6 tmp;
                int res = inet_pton(AF_INET6,addr.data(),&tmp);
                if(res>0)
                    config.accepted_addresses_.insert(std::string(addr));
                else return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"invalid address",AT_ERROR_ACTION::CONTINUE,addr);
            }
        }
        return ErrorCode::NONE;
    }

    ErrorCode add_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = ServerConfig::instance().parse(input);
        if(err!=ErrorCode::NONE){
            ServerConfig::config().reset();
            return err;
        }
        else if(!Application::config().add_server_config(std::move(*ServerConfig::config().release())))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server config already exists",AT_ERROR_ACTION::CONTINUE,input);
        else return ErrorCode::NONE;
    }

    ErrorCode setup_notifier(const std::vector<std::string>& input) noexcept{
        ErrorCode err = ServerConfig::instance().parse(input);
        if(err!=ErrorCode::NONE){
            ServerConfig::config().reset();
            return err;
        }
        else if(!Application::config().setup_server_config(std::move(*ServerConfig::config().release())))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server config doesn't exists",AT_ERROR_ACTION::CONTINUE,input);
        else return ErrorCode::NONE;
    }

    ErrorCode set_notifier(const std::string& input) noexcept{
        if(!Application::config().set_current_server_config(input))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server config name don't exists",AT_ERROR_ACTION::CONTINUE,input);
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
    }
    ErrorCode launch_notifier(const std::string& name) noexcept{
        if(hProgram){
            if(name.empty())
                hProgram->launch_server();
            else{
                if(!Application::config().set_current_server_config(name))
                    return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"removing server config failed (config don't exists)",AT_ERROR_ACTION::CONTINUE,name);
            }
        }
        else return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"errorness program initialization. Could not launch server",AT_ERROR_ACTION::ABORT);
        return ErrorCode::NONE;
    }

    ErrorCode shutdown_notifier(bool wait) noexcept{
        return ErrorCode::NONE;
    }

    ErrorCode closing_notifier(bool wait) noexcept{
        if(hProgram)
            hProgram->close_server(wait);
        else return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Nothing to close",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
    }

    ErrorCode get_current_config() noexcept{
        Application::config().get_current_server_config().print_server_config(std::cout);
        return ErrorCode::NONE;
    }

    ErrorCode host_notifier(const std::string& input,network::server::Settings& set) noexcept{   
        if(network::is_correct_address(input)){
            set.host=input;
            return ErrorCode::NONE;
        }
        else return ErrorPrint::print_error(ErrorCode::INVALID_HOST_X1,"",AT_ERROR_ACTION::CONTINUE,input);
    }

    ErrorCode port_notifier(int input,network::server::Settings& set) noexcept{
        if(!input<1024 || input>std::numeric_limits<uint16_t>::max())
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"port definition. Must be bigger than 1024",AT_ERROR_ACTION::CONTINUE,input);
        return ErrorCode::NONE;
    }

    ErrorCode timeout_notifier(int timeout,network::server::Settings& set) noexcept{
        if(timeout<0)
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"timeout definition",AT_ERROR_ACTION::CONTINUE,timeout);
        else set.timeout_seconds_ = timeout;
    }

    ErrorCode protocol_notifier(const std::string& input,network::server::Settings& set) noexcept{
        if(!getprotobyname(input.c_str()))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"protocol definition",AT_ERROR_ACTION::CONTINUE,input);
        else set.protocol = input;
    }

    ErrorCode ServerConfig::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        config() = std::make_unique<network::server::Config>();
        config()->name_ = vm.at("name").as<std::string>();

        err_ = host_notifier(vm.at("host").as<std::string>(),config()->settings_);
        if(err_!=ErrorCode::NONE)
            return err_;
        if(vm.contains("accepted-addresses")){
            err_=add_addresses(vm.at("accepted-addresses").as<std::vector<std::string>>(),*config());
        }

        err_ = port_notifier(vm.at("port").as<int>(),config()->settings_);
        if(err_!=ErrorCode::NONE)
            return err_;

        if(vm.contains("timeout")){
            err_=timeout_notifier(vm.at("timeout").as<int>(),config()->settings_);
            if(err_!=ErrorCode::NONE)
                return err_;
        }
        return ErrorCode::NONE;
    }
}