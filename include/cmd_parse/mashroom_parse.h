#pragma once
#include "cmd_parse/cmd_def.h"
#include "cmd_parse/index_parse.h"
#include "cmd_parse/extract_parse.h"
#include "cmd_parse/integrity_parse.h"
#include "cmd_parse/config_parse.h"
#include "cmd_parse/contains_parse.h"
#include "cmd_parse/functions.h"
#include "cmd_parse/network.h"
#include "cmd_parse/server_config_parse.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "program/mashroom.h"
#include <CLI/CLI.hpp>

namespace parse{
    namespace po = boost::program_options;
    ErrorCode extract_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode index_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode integrity_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode contains_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode config_notifier(const std::vector<std::string>& input) noexcept;
    ErrorCode save_notifier() noexcept;
    ErrorCode exit_notifier(const std::string& input) noexcept;

    void init_extract(CLI::App* cli) noexcept{

    }
    void init_index(CLI::App* cli) noexcept{
    }
    void init_integrity(CLI::App* cli) noexcept{
    }
    void init_contains(CLI::App* cli) noexcept{
    }
    void init_config(CLI::App* cli) noexcept{
    }
    void init_network(CLI::App* cli) noexcept{
    }
    void init_save(CLI::App* cli) noexcept{
    }
    void init_help(CLI::App* cli) noexcept{
    }
    void init_exit(CLI::App* cli) noexcept{
    }

    class Mashroom{
        Mashroom(){}
        virtual void init() noexcept override final;
        void print_help(std::ostream& os,const std::string& option,std::span<const std::string> args) const noexcept;
    };
}