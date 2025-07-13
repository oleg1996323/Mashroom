#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/server_parse.h"
#include "cmd_parse/cmd_def.h"
#include "sys/application.h"
namespace parse{
    class ConfigCommandsArguments:public AbstractCLIParser<parse::ConfigCommandsArguments>{
        friend AbstractCLIParser;
        std::vector<std::string_view> commands_;
        ConfigCommandsArguments();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
    };

    ErrorCode add_user_config_notifier(const std::vector<std::string>& args);
    ErrorCode remove_user_config_notifier(const std::string& name);
    ErrorCode redefine_user_config_notifier(const std::string& name,const std::vector<std::string>& commands);

    class Config:public AbstractCLIParser<parse::Config>{
        friend AbstractCLIParser;
        Config();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
    };
}