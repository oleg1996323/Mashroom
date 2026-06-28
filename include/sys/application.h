#pragma once
#include <memory>
#include <string_view>
#include <string>
#include "sys/log_err.h"
#include "sys/config.h"
#include <CLI/CLI.hpp>

class Config;
class LogError;
class Application;

class Application{
    std::unique_ptr<Config> conf_;
    std::unique_ptr<LogError> logger_;
    std::string_view program_name_;
    public:
    Application();
    static Application& app();
    static LogError& log();
    static Config& config();
    static std::string_view program_name();
    private:

    // static std::unique_ptr<ProxyDataInfo> d_info_;
};

Application& app();
LogError& log();