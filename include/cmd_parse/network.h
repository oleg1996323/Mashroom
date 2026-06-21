#pragma once
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/log_err.h"
#include <set>
#include <CLI/CLI.hpp>

namespace parse{

    void init_network(CLI::App* app);


    class Network{
        CLI::App* app_;
        CLI::App * launch_=app_->add_subcommand(
                "launch-server","");
        CLI::App * close_=app_->add_subcommand(
                "close","close all connections with launched server");
        CLI::App * suspend_=app_->add_subcommand(
                "suspend","suspend all connections with launched server");
        uint16_t timeout_ = 0;
        public:
        Network(CLI::App* app);
        void execute();
    };
}