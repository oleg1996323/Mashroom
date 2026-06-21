#include "application.h"

Application::Application():
                conf_(std::make_unique<Config>()),
                logger_(std::make_unique<LogError>())
{}

Application& Application::app(){
    static std::unique_ptr<Application> app;
    if(!app)
        app=std::make_unique<Application>();
    return *app;
}
LogError& Application::log(){
    return *(app().logger_);
}
Config& Application::config(){
    return *(app().conf_);
}
std::string_view Application::program_name(){
    return program_invocation_name;
}

Application& app(){
    return Application::app();
}

LogError& log(){
    return app().log();
}
