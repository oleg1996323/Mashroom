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

namespace parse{
    UserConfig::UserConfig():AbstractCLIParser("Config arguments:"){}

    void UserConfig::init() noexcept{
        add_options("name,N",po::value<std::string>()->required(),"Configure the program")
        ("cap-upd-time-interval",po::value<std::string>()/*add notifier parsing TimeInterval*/,"");
        ("mashroom-upd-time-interval",po::value<std::string>(),"Sets the time interval of Mashroom's updates");
        define_uniques();
    }
    ErrorCode UserConfig::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        /**
         * @todo add new config Settings (see /sys)
         */
        if(vm.contains("add")){
            
        }
    }

    ErrorCode add_user_config_notifier(const std::vector<std::string>& args){
        return UserConfig::instance().parse(args);
    }
    ErrorCode remove_user_config_notifier(const std::string& name){
        if(!Application::config().remove_user_config(name))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",AT_ERROR_ACTION::CONTINUE,name);
        return ErrorCode::NONE;
    }

    ErrorCode redefine_user_config_notifier(const std::vector<std::string>& args){
        UserConfig::instance().parse(args);
        if(UserConfig::instance().settings()){
            if(!Application::config().change_user_config(
                        *UserConfig::instance().settings()))
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "config name doesn't exists",
                    AT_ERROR_ACTION::CONTINUE,UserConfig::instance().settings()->name_);
        }
        else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
    }

    Config::Config():AbstractCLIParser("Config options:"){}

    void Config::init() noexcept{
        add_options("add,A",po::value<std::vector<std::string>>(),"Configure the program")
        ("remove,R",po::value<std::string>()->notifier([this](const std::string& name){
            err_ = remove_user_config_notifier(name);
        }),"")
        ("redefine",po::value<std::vector<std::string>>(),"")
        ("get-config",po::value<std::string>(),"Print the confguration with indicated name")
        ("print-all-configs","Print all accessible configurations");
        add_options_instances("server","Configure the server settings",ServerAction::instance());
        define_uniques();
    }
    ErrorCode Config::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        if(vm.contains("add"))
            return add_user_config_notifier(args);
        else if(vm.contains("redefine"))
            return redefine_user_config_notifier(vm.at("redefine").as<std::vector<std::string>>());
        else if(vm.contains("get-config")){
            if(app().config().has_config_name(vm.at("get-config").as<std::string>()))
                std::cout<<to_json(app().config().
                get_user_config(vm.at("get-config").as<std::string>()))<<
                std::endl;
            else ErrorPrint::print_error(ErrorCode::CONFIG_NAME_DOESNT_EXISTS_X1,
                "",AT_ERROR_ACTION::CONTINUE,vm.at("get-config").as<std::string>());
            return ErrorCode::NONE;
        }
        else if(vm.contains("print-all-configs")){
            for(auto& config:app().config().get_user_configs())
                std::cout<<to_json(config)<<std::endl;
            return ErrorCode::NONE;
        }
        else if(vm.contains("server"))
            return ServerAction::instance().parse(args);
        else
            return try_notify(vm);
    }
}