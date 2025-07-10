#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/server_parse.h"

ErrorCode config_parse(const std::vector<std::string_view>& args);

#include "cmd_parse/cmd_def.h"
namespace parse{
    class ConfigCommandsArguments:public BaseParser<parse::ConfigCommandsArguments>{
        std::vector<std::string_view> commands_;
        ConfigCommandsArguments():BaseParser("Config arguments:"){}

        virtual void init() noexcept override final{
            descriptor_.add_options()
            ("name,N",po::value<std::string>(),"Configure the program")
            ("capupd-time-interval,C",po::value<std::string>()/*add notifier parsing TimeInterval*/,"")
            ("checkupd-time-interval,U",po::value<std::string>(),"Sets the time interval of Mashroom's updates");
            define_uniques();
        }
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{
            if(vm.contains("add")){
                
            }
        }
    };

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

    class Config:public BaseParser<parse::Config>{
        Config():BaseParser("Config options:"){}

        virtual void init() noexcept override final{
            descriptor_.add_options()
            ("add,A",po::value<std::vector<std::string>>(),"Configure the program")
            ("remove,R",po::value<std::string>()->notifier([&](const std::string& name){
                err_ = remove_user_config_notifier(name);
            }),"")
            ("redefine",po::value<std::string>(),"")
            ("server","Configure the server settings");
            define_uniques();
        }
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final{
            if(vm.contains("add"))
                return add_user_config_notifier(args);
            else if(vm.contains("redefine"))
                return redefine_user_config_notifier(vm.at("redefine").as<std::string>(),args);
            else if(vm.contains("server"))
                return ServerAction::instance().parse(args);
            else
                return try_notify(vm);
        }
    };
}