#include "application.h"
#include "OsterLib/log.h"

Application::Application():
                conf_(std::make_unique<Config>()),
                logger_(std::make_unique<osterlib::Log>())
{}

Application& Application::app(){
    static std::unique_ptr<Application> app;
    if(!app)
        app=std::make_unique<Application>();
    return *app;
}
osterlib::Log& Application::log(){
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

osterlib::Log& log(){
    return app().log();
}
