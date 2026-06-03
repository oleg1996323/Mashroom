#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/network.h"
#include "sys/application.h"
#include "config/server.h"
#include "config/client.h"
#include "config/user.h"

namespace parse{
    class SystemConfig:public AbstractCLIParser<parse::SystemConfig>{
        friend AbstractCLIParser;
        SystemConfig();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
    };
    class UserConfig:public AbstractCLIParser<parse::UserConfig>{
        friend AbstractCLIParser;
        UserConfig();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
    };
    class NetworkConfig:public AbstractCLIParser<parse::NetworkConfig>{
        friend AbstractCLIParser;
        NetworkConfig();
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
    };
    
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