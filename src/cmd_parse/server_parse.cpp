#include <cmd_parse/server_parse.h>
#include <functions.h>
#include <sys/application.h>
#include <sys/config.h>
#include <sys/log_err.h>

using namespace translate::token;

ErrorCode server_settings_parse(std::string_view key, std::string_view arg,server::Settings& set){
    ErrorCode err;
    switch(translate_from_txt<ServerConfigCommands>(key)){
        case ServerConfigCommands::HOST:{
            std::string arg_string;
            arg_string = arg;
            {
                sockaddr_in tmp;
                int res = inet_pton(AF_INET,arg_string.c_str(),&tmp);
                if(res>0){
                    set.host=arg;
                    break;
                }
            }
            {
                sockaddr_in6 tmp;
                int res = inet_pton(AF_INET6,arg_string.c_str(),&tmp);
                if(res>0){
                    set.host=arg;
                    break;
                }
            }
            break;
        }
        case ServerConfigCommands::SERVICE:{
            std::vector<std::string_view> servname_protocol = split(arg,":");
            if(servname_protocol.empty()){
                err=ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"service definition error",AT_ERROR_ACTION::CONTINUE,arg);
                return err;
            }
            else if(servname_protocol.size()==1){
                std::string service;
                service = servname_protocol.front();
                auto serv =getservbyname(service.c_str(),NULL);
                if(!serv){
                    std::optional<short> port = from_chars<short>(arg,err);
                    if(!port.has_value() || err!=ErrorCode::NONE){
                        err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"service definition",AT_ERROR_ACTION::CONTINUE,arg);
                        return err;
                    }
                }
                else{
                    set.service=arg;
                    set.protocol=serv->s_proto;
                    break;
                }
            }
            else if(servname_protocol.size()==2){
                std::string service;
                service = servname_protocol.front();
                std::string protocol;
                protocol = servname_protocol.back();
                auto serv =getservbyname(service.c_str(),protocol.c_str());
                if(!serv){
                    err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"service definition",AT_ERROR_ACTION::CONTINUE,arg);
                    return err;
                }
                else{
                    set.service=arg;
                    set.protocol=serv->s_proto;
                    break;
                }
            }
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"incorrect port",AT_ERROR_ACTION::CONTINUE,arg);
            break;
        }
        case ServerConfigCommands::PORT:{
            std::optional<short> port = from_chars<short>(arg,err);
            if(!port.has_value() || err!=ErrorCode::NONE){
                err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"port definition",AT_ERROR_ACTION::CONTINUE,arg);
                return err;
            }
            else set.port = arg;
            break;
        }
        case ServerConfigCommands::TIMEOUT:{
            auto timeout = from_chars<int>(arg,err);
            if(!timeout.has_value() || err!=ErrorCode::NONE || timeout.value()<server::min_timeout_seconds)
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"timeout definition",AT_ERROR_ACTION::CONTINUE,arg);
            else set.timeout_seconds_ = timeout.value();
            break;
        }
        case ServerConfigCommands::PROTOCOL:{
            if(!getprotobyname(std::string(arg).c_str())){
                err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"protocol definition",AT_ERROR_ACTION::CONTINUE,arg);
                return err; 
            }
            else set.protocol = arg;
            break;
        }
        default:{
            err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined server setting argument",AT_ERROR_ACTION::CONTINUE,arg);
            return err;
        }
    }
    return err;
}

ErrorCode server_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands_saved;
    std::string_view config_name;
    ErrorCode err = ErrorCode::NONE; 
    switch(translate_from_txt<ServerAction>(input.at(0))){
        case ServerAction::ADD:{
            server::ServerConfig config;
            err = server_config(std::vector<std::string_view>(input.begin()+1,input.end()),config);
            if(err!=ErrorCode::NONE)
                return err;
            else Application::config().add_server_config(std::move(config));
            break;
        }
        case ServerAction::SET:{
            if(input.size()<2){
                err=ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"not defined server config name",AT_ERROR_ACTION::CONTINUE);
                return err;
            }
            if(input.size()>2){
                err=ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                return err;
            }
            if(!Application::config().set_current_server_config(input.at(1))){
                err=ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server config name don't exists",AT_ERROR_ACTION::CONTINUE,input.at(1));
                return err;
            }
            break;
        }
        case ServerAction::REMOVE:
            if(input.size()>1){
                if(input.size()>2){
                    err = ErrorPrint::print_error(ErrorCode::TO_MANY_ARGUMENTS,"only server config name must be indicated",AT_ERROR_ACTION::CONTINUE);
                    return err;
                }
                else
                    if(!Application::config().remove_server_config(input.at(1))){
                        err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"removing server config failed (indicated \"default\" or config don't exists)",AT_ERROR_ACTION::CONTINUE,input.at(1));
                        return err;
                    }
            }
            else {
                err = ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"server config name must be indicated",AT_ERROR_ACTION::CONTINUE);
                return err;
            }
            break;
        case ServerAction::LAUNCH:
            if(hProgram)
                hProgram->launch_server();
            break;
        case ServerAction::SHUTDOWN:
            break;
        case ServerAction::CLOSE:
            if(hProgram)
                hProgram->close_server();
            break;
        case ServerAction::COLLAPSE:
            if(hProgram)
                hProgram->collapse_server();
            break;
        case ServerAction::GET_CURRENT_CONFIG:
            Application::config().get_current_server_config().print_server_config(std::cout);
            break;
        default:
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined server mode argument",AT_ERROR_ACTION::CONTINUE,input.at(0));
    }
    return err;
}

ErrorCode server_config(const std::vector<std::string_view>& input,server::ServerConfig& config){
    ErrorCode err = ErrorCode::NONE;
    int i=0;
    if(input.size()>1){
        if(translate_from_txt<ServerConfigCommands>(input.at(i))!=ServerConfigCommands::NAME){
            err=ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server adding config. Must be \"-name <config-name>\"",AT_ERROR_ACTION::CONTINUE,input.at(i));
            return err;
        }
        else{
            ++i;
            config.name_ = input.at(i++);
            assert(!config.name_.contains(' '));
        }
    }
    else{
        err = ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"server adding config",AT_ERROR_ACTION::CONTINUE);
        return err;
    }
    for(;i<input.size()-1;++i){
        switch(translate_from_txt<ServerConfigCommands>(input.at(i++))){
            case ServerConfigCommands::ACCEPTED_ADDRESSES:{
                if(input.at(i).starts_with("[") && input.at(i).ends_with("]")){
                    std::vector<std::string_view> addresses = split(input.at(i),";");
                    for(auto addr : addresses){
                        {
                            sockaddr_in tmp;
                            int res = inet_pton(AF_INET,input.at(i).data(),&tmp);
                            if(res>0){
                                config.accepted_addresses_.insert(std::string(addr));
                                break;
                            }
                        }
                        {
                            sockaddr_in6 tmp;
                            int res = inet_pton(AF_INET6,input.at(i).data(),&tmp);
                            if(res>0){
                                config.accepted_addresses_.insert(std::string(addr));
                                break;
                            }
                        }
                    }
                }
                else return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"accepted addresses",AT_ERROR_ACTION::CONTINUE,input.at(i));
                break;
            }
            case ServerConfigCommands::HOST:
            case ServerConfigCommands::PORT:
            case ServerConfigCommands::SERVICE:
            case ServerConfigCommands::TIMEOUT:
            case ServerConfigCommands::PROTOCOL:
                err = server_settings_parse(input.at(i-1),input.at(i),config.settings_);
                if(err!=ErrorCode::NONE)
                    return err;
                break;
            case ServerConfigCommands::NAME:{
                err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"server adding config. Name already defined",AT_ERROR_ACTION::CONTINUE,input.at(i-1));
                return err;
            }
            default:
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined server config argument",AT_ERROR_ACTION::CONTINUE,input.at(2));
        }
    }
    if(!config){
        err = ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"server adding config. Not full config settings definition",AT_ERROR_ACTION::CONTINUE);
        return err;
    }
    return err;
}