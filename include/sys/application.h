#pragma once
#include "log_err.h"

class Application{
    public:

    static LogError& log(){
        return logger_;
    }
    static Application& app(){
        return *app_;
    }
    private:
    static Application* app_;
    static LogError logger_;
};

static Application& app(){
    return Application::app();
}