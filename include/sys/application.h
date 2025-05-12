#pragma once
#include <memory>
#include <string_view>
#include <string>
#include "log_err.h"
#include "config.h"

class Config;
class LogError;
class Application;

class Application{
    static std::unique_ptr<Application> app_;
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

    // static std::unique_ptr<DataInfo> d_info_;
};

inline std::unique_ptr<Application> Application::app_=std::make_unique<Application>();
Application& app();
LogError& log();