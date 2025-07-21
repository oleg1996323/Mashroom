#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/server_parse.h"
#include "cmd_parse/cmd_def.h"
#include "sys/application.h"
namespace parse{
    class UserConfig:public AbstractCLIParser<parse::UserConfig>{
        std::unique_ptr<user::Settings> settings_;
        friend AbstractCLIParser;
        UserConfig();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        public:
        const std::unique_ptr<user::Settings>& settings() const{
            return settings_;
        }
    };

    ErrorCode add_user_config_notifier(const std::vector<std::string>& args);
    ErrorCode remove_user_config_notifier(const std::string& name);
    ErrorCode redefine_user_config_notifier(const std::vector<std::string>& commands);

    class Config:public AbstractCLIParser<parse::Config>{
        friend AbstractCLIParser;
        std::unique_ptr<Config> hConfig;
        Config();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        const std::unique_ptr<Config>& config() const noexcept{
            return hConfig;
        }
    };
}