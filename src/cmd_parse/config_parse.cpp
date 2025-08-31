#include "cmd_parse/config_parse.h"
#include <thread>

#include "sys/error_code.h"
#include "sys/error_print.h"
#include "proc/index.h"
#include "sys/application.h"
#include "sys/config.h"
#include "cmd_parse/cmd_def.h"
#include "cmd_parse/functions.h"
#include "functions.h"
#include "cmd_parse/index_parse.h"
#include "cmd_parse/integrity_parse.h"
#include "cmd_parse/extract_parse.h"
#include <boost/program_options.hpp>

namespace parse{
    ProgramConfigOptions::ProgramConfigOptions():AbstractCLIParser("Program configuration options "){}

    void ProgramConfigOptions::init() noexcept{
        add_options("name,N",po::value<std::string>(),"Configuration name")
        ("index-upd-time-period",po::value<TimePeriod>()->default_value(user::default_config().index_update_ti_),"Sets the time period of index update")
        ("mashroom-upd-time-period",po::value<TimePeriod>()->default_value(user::default_config().mashroom_update_ti_),"Sets the time period of Mashroom's updates");
        define_uniques();
    }
    ErrorCode ProgramConfigOptions::execute(vars& vm,const std::vector<std::string>& args) noexcept{
        /**
         * @todo add new config Settings (see /sys)
         */
        if(!settings_)
            return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"undefined user-configuration's object",AT_ERROR_ACTION::CONTINUE);

        if(vm.contains("name")){
            settings_->name_ = vm.at("name").as<std::string>();
        }
        else{
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"configuration name",AT_ERROR_ACTION::CONTINUE);
        }
        try{
            if(vm.contains("index-upd-time-period"))
                settings_->index_update_ti_ = vm.at("index-upd-time-period").as<TimePeriod>();
            if(vm.contains("mashroom-upd-time-period"))
                settings_->mashroom_update_ti_ = vm.at("mashroom-upd-time-period").as<TimePeriod>();
            return ErrorCode::NONE;
        }
        catch(const std::invalid_argument& err){
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,err.what());
        }
    }

    ErrorCode add_user_config_notifier(const std::vector<std::string>& args){
        return ProgramConfigOptions::instance().parse(args);
    }
    ErrorCode remove_user_config_notifier(const std::string& name){
        if(!Application::config().remove_user_config(name))
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                "config name doesn't exists",AT_ERROR_ACTION::CONTINUE,name);
        return ErrorCode::NONE;
    }

    ErrorCode redefine_user_config_notifier(const std::vector<std::string>& args){
        ProgramConfigOptions::instance().parse(args);
        if(ProgramConfigOptions::instance().settings()){
            if(!Application::config().setup_user_config(
                        std::move(*ProgramConfigOptions::instance().settings().release())))
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "config name doesn't exists",
                    AT_ERROR_ACTION::CONTINUE,ProgramConfigOptions::instance().settings()->name_);
        }
        else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
    }

    ProgramConfig::ProgramConfig():AbstractCLIParser("Config options:"){}

    void ProgramConfig::init() noexcept{
        add_options_instances("add,A",po::value<std::vector<std::string>>(),"Configure the program",ProgramConfigOptions::instance());
        add_options
        ("remove,R",po::value<std::string>()->notifier([this](const std::string& name){
            err_ = remove_user_config_notifier(name);
        }),"")
        ("redefine",po::value<std::vector<std::string>>(),"")
        ("get-config",po::value<std::string>(),"Print the confguration with indicated name")
        ("print-all","Print all accessible program configurations");
        define_uniques();
    }
    ErrorCode ProgramConfig::execute(vars& vm,const std::vector<std::string>& args) noexcept{
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
        else if(vm.contains("print-all")){
            for(auto& config:app().config().get_user_configs())
                std::cout<<to_json(config)<<std::endl;
            return ErrorCode::NONE;
        }
        else
            return try_notify(vm);
    }
}