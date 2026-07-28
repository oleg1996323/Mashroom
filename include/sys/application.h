#pragma once
#include <memory>
#include <string_view>
#include <string>
#include "config.h"
#include <CLI/CLI.hpp>
#include "OsterLib/log.h"

class Config;
class Application;

class Application{
    std::unique_ptr<Config> conf_;
    std::unique_ptr<osterlib::Log> logger_;
    std::string_view program_name_;
    public:
    Application();
    static Application& app();
    static osterlib::Log& log();
    static Config& config();
    static std::string_view program_name();
    private:

    // static std::unique_ptr<ProxyDataInfo> d_info_;
};

Application& app();
osterlib::Log& log();