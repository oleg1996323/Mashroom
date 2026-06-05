#pragma once
#include <vector>
#include <string_view>
#include "sys/error_code.h"
#include "cmd_parse/network.h"
#include "sys/application.h"
#include "config/server.h"
#include "config/client.h"
#include "config/user.h"
#include <CLI/CLI.hpp>

namespace parse{
    class SystemConfig{
        SystemConfig(CLI::App* app);
        void execute();
    };
    class UserConfig{
        CLI::App* app_;
        CLI::Option* output_files_root_dir_;
        CLI::Option* index_upd_ti_;
        CLI::Option* mashroom_upd_ti_;
        CLI::Option* output_fmt_default_;
        UserConfig(CLI::App* app):
        app_(app)
        {
            output_files_root_dir_=app_
            ->add_option("--output-root-dir",
            "the root directory where user files will be placed")
            ->default_val("~/Mashroom_output/")
            ->capture_default_str();
            index_upd_ti_=app_
            ->add_option("--index-update-ti",
            "index update time interval")
            ->default_val([](){
                std::error_code err;
                return DateTimeDiff(err,days(7));}())
            ->capture_default_str();
            mashroom_upd_ti_=app_
            ->add_option("--mashroom-update-ti",
            "Mashroom update time interval")
            ->default_val([](){
                std::error_code err;
                return DateTimeDiff(err,days(7));}())
            ->capture_default_str();
        }
        void execute();
    };
    class NetworkConfig{
        NetworkConfig(CLI::App* app);
        void execute();
    };
    
    class Configuration{
        CLI::App* app_;
        CLI::App * user_=app_->add_subcommand(
                "user","user configuration");
        CLI::App * close_=app_->add_subcommand(
                "system","system configuration");
        CLI::App * suspend_=app_->add_subcommand(
                "network","network configuration");
        Configuration(CLI::App* app);
        void execute();
    };
}