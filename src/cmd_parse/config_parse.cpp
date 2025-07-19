#include "cmd_parse/config_parse.h"
#include <thread>

#include "sys/error_code.h"
#include "sys/error_print.h"
#include "proc/capitalize.h"
#include "sys/application.h"
#include "sys/config.h"
#include "cmd_parse/cmd_def.h"
#include "cmd_translator.h"
#include "cmd_parse/functions.h"
#include "functions.h"
#include "cmd_parse/capitalize_parse.h"
#include "cmd_parse/integrity_parse.h"
#include "cmd_parse/extract_parse.h"
#include <boost/program_options.hpp>

using namespace translate::token;

ErrorCode config_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands_saved;
    ConfigAction action = ConfigAction::UNDEF;
    std::string_view config_name;
    ErrorCode err = ErrorCode::NONE; 
    switch(translate_from_txt<ConfigAction>(input.at(0))){
        case ConfigAction::ADD:
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::ADD;
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
        case ConfigAction::REMOVE :{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::REMOVE;
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
        }
        case ConfigAction::ADDSET:{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::ADDSET;
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
        }
        case ConfigAction::SET:{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::SET;
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
            if(!input.at(1).starts_with('-'))
                config_name = input.at(1);
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
            if(app().config().has_config_name(input.at(1)))
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Already defined user-commands config name",AT_ERROR_ACTION::CONTINUE,input.at(1));
            break;
        }
        case ConfigAction::REDEFINE :{
            if(action==ConfigAction::UNDEF){
                action = ConfigAction::REDEFINE;
            }
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
            if(!input.at(1).starts_with('-'))
                config_name = input.at(1);
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
        }
        case ConfigAction::GET:{
            if(input.size()>1)
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input.at(1)); 
            if(input.at(1)=="."){
                for(auto& [name,commands]:app().config().get_user_configs()){
                    std::cout<<name<<"-> ";
                    for(const std::string& command:commands){
                        std::cout<<command<<' ';
                    }
                    std::cout.seekp(-1);
                    std::cout<<std::endl;
                }
            }
            else if(!input.at(1).starts_with('-')){
                if(!app().config().has_config_name(input.at(1)))
                    return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"User-commands config name doesn't exists",AT_ERROR_ACTION::CONTINUE,input.at(1));
                std::cout<<input.at(1)<<"-> ";
                for(auto& command:app().config().get_user_config(input.at(1))){
                    std::cout<<command<<' ';
                    std::cout.seekp(-1);
                    std::cout<<std::endl;
                }
            }
            else 
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(1));

            }
        default:{
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(0));
            return ErrorCode::COMMAND_INPUT_X1_ERROR;
        }
    }
    if(err!=ErrorCode::NONE)
        return err;
    Application::config().add_user_config(config_name,commands_saved);
    return err;
}

namespace parse{
    ConfigCommandsArguments::ConfigCommandsArguments():AbstractCLIParser("Config arguments:"){}

    void ConfigCommandsArguments::init() noexcept{
        add_options("name,N",po::value<std::string>()->required(),"Configure the program")
        ("cap-upd-time-interval",po::value<std::string>()/*add notifier parsing TimeInterval*/,"");
        ("mashroom-upd-time-interval,U",po::value<std::string>(),"Sets the time interval of Mashroom's updates");
        define_uniques();
    }
    ErrorCode ConfigCommandsArguments::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        /**
         * @todo add new config Settings (see /sys)
         */
        if(vm.contains("add")){
            
        }
    }

    ErrorCode add_user_config_notifier(const std::vector<std::string>& args){
        return ConfigCommandsArguments::instance().parse(args);
    }
    ErrorCode remove_user_config_notifier(const std::string& name){
        if(!Application::config().remove_user_config(name))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",AT_ERROR_ACTION::CONTINUE,name);
        return ErrorCode::NONE;
    }

    ErrorCode redefine_user_config_notifier(const std::string& name,const std::vector<std::string>& commands){
        if(!Application::config().change_user_config(name,commands))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",AT_ERROR_ACTION::CONTINUE,name);
        return ErrorCode::NONE;
    }

    Config::Config():AbstractCLIParser("Config options:"){}

    void Config::init() noexcept{
        add_options("add,A",po::value<std::vector<std::string>>(),"Configure the program")
        ("remove,R",po::value<std::string>()->notifier([this](const std::string& name){
            err_ = remove_user_config_notifier(name);
        }),"")
        ("redefine",po::value<std::string>(),"");
        add_options_instances("server","Configure the server settings",ServerAction::instance());
        define_uniques();
    }
    ErrorCode Config::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        if(vm.contains("add"))
            return add_user_config_notifier(args);
        else if(vm.contains("redefine"))
            return redefine_user_config_notifier(vm.at("redefine").as<std::string>(),args);
        else if(vm.contains("server"))
            return ServerAction::instance().parse(args);
        else
            return try_notify(vm);
    }
}