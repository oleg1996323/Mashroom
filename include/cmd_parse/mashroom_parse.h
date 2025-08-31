#pragma once
#include "cmd_parse/cmd_def.h"
#include "cmd_parse/index_parse.h"
#include "cmd_parse/extract_parse.h"
#include "cmd_parse/integrity_parse.h"
#include "cmd_parse/config_parse.h"
#include "cmd_parse/contains_parse.h"
#include "cmd_parse/functions.h"
#include "cmd_parse/server_parse.h"
#include "cmd_parse/server_config_parse.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include <boost/program_options.hpp>
#include "program/mashroom.h"

namespace parse{
    namespace po = boost::program_options;
    ErrorCode extract_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode index_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode integrity_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode contains_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode save_notifier() noexcept;
    ErrorCode exit_notifier(const std::string& input) noexcept;

    class Mashroom:public AbstractCLIParser<parse::Mashroom>{
        friend AbstractCLIParser;
        Mashroom():AbstractCLIParser("Mashroom options"){}
        virtual void init() noexcept override final;
        virtual ErrorCode execute(vars& vm,const std::vector<std::string>& args) noexcept override final;
        void print_help(std::ostream& os,const std::string& option,std::span<const std::string> args) const noexcept;
    };
}