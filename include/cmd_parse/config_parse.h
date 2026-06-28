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
        CLI::App* app_;
        std::string log_dir_val_;
        std::string config_dir_val_;
        std::string cache_files_dir_val_;
        std::string network_files_dir_val_;
        public:
        SystemConfig(CLI::App* app);
        void execute();
    };
    
    class NetworkConfig{
        CLI::App* app_;
        public:
        NetworkConfig(CLI::App* app);
        void execute();
    };
    
    class Configuration{
        CLI::App* app_;
        public:
        Configuration(CLI::App* app);
        void execute();
    };
}