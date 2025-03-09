#pragma once
#include <memory>
#include <string_view>
#include <string>
#include "log_err.h"
#include "config.h"
#include "data_info.h"

class Application{
    public:
    static LogError& log(){
        return *logger_;
    }
    static Application& app(){
        return *app_;
    }
    static std::string_view program_name(){
        return program_invocation_name;
    }
    private:
    std::string_view program_name_;
    static std::unique_ptr<Application> app_;
    static std::unique_ptr<Config> conf_;
    static std::unique_ptr<LogError> logger_;
    static std::unique_ptr<DataInfo> d_info_;
};

static Application& app(){
    return Application::app();
}

static LogError& log(){
    return app().log();
}