#pragma once
#include "network/server.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/config.h"
#include "sys/log_err.h"
#include "cmd_parse/cmd_def.h"
#include <boost/program_options.hpp>
#include <set>
#include "cmd_parse/functions.h"


namespace parse{

    ErrorCode launch_notifier(const std::string&) noexcept;
    ErrorCode shutdown_notifier(bool wait) noexcept;
    ErrorCode closing_notifier(bool wait) noexcept;
    ErrorCode get_current_config() noexcept;

    class ServerAction:public AbstractCLIParser<ServerAction>{
        friend AbstractCLIParser;
        ServerAction():AbstractCLIParser("Server use:"){}

        virtual void init() noexcept override final;

        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& tokens) noexcept override final;
    };
}