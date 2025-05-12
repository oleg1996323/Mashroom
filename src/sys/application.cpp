#include "application.h"

Application::Application():
                conf_(std::make_unique<Config>()),
                logger_(std::make_unique<LogError>())
{}

Application& Application::app(){
    return *app_;
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
