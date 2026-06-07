#pragma once
#include "cmd_parse/config_parse.h"
#include "cmd_parse/network.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "program/mashroom.h"
#include <CLI/CLI.hpp>

namespace parse{
    namespace po = boost::program_options;
    class Mashroom{
        CLI::App* app_ = &Application::command_line();
        CLI::App* index_;
        CLI::App* extract_;
        CLI::App* contains_;
        CLI::App* integrity_;
        CLI::App* config_;
        CLI::App* server_;
        CLI::App* save_;
        CLI::App* help_;
        CLI::App* exit_;
        Mashroom(){
            index_ = app_->add_subcommand("index",
                "read specified files and register "s+
                "the contained data properties and "+
                "data positions. Organize the data "+
                "by defined hierarchy from unique "+
                "massive archive.")->require_subcommand(1)->
                alias("-I");
            extract_ = app_->add_subcommand("extract","Extract specified data.")
            ->require_subcommand(1);
            contains_ = app_->add_subcommand("contains",
            "check if indexed data contains the data specified by properties")
            ->require_subcommand(1);
            integrity_ = app_->add_subcommand("check",
            "check the integrity (dimensional and temporal) of indexed data and detect the corrupted files of different format.")
            ->require_subcommand(1);
            auto save_cmd = app_->add_subcommand("save",
                "save the current instance (data, configurations)");
            config_=app_->add_subcommand("configure",
            "set configurations");
            server_=app_->add_subcommand("server",
            "server activities");
            help_ = app_->add_subcommand("help","Show help");
            exit_ = app_->add_subcommand("exit","Exit from program");
            exit_->add_flag("--save","flag if saving is needed before exit");
        }
        void execute();
        void print_help(std::ostream& os) const noexcept;
    };
}