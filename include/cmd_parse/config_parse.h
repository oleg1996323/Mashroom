#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/server_parse.h"
#include "cmd_parse/cmd_def.h"
#include "sys/application.h"
namespace parse{
    class ProgramConfigOptions:public AbstractCLIParser<parse::ProgramConfigOptions>{
        std::unique_ptr<user::Settings> settings_;
        friend AbstractCLIParser;
        ProgramConfigOptions();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        public:
        std::unique_ptr<user::Settings>& settings(){
            return settings_;
        }
    };

    ErrorCode add_user_config_notifier(const std::vector<std::string>& args);
    ErrorCode remove_user_config_notifier(const std::string& name);
    ErrorCode redefine_user_config_notifier(const std::vector<std::string>& commands);

    class ProgramConfig:public AbstractCLIParser<parse::ProgramConfig>{
        friend AbstractCLIParser;
        std::unique_ptr<ProgramConfig> hConfig;
        ProgramConfig();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        const std::unique_ptr<ProgramConfig>& config() const noexcept{
            return hConfig;
        }
    };
}