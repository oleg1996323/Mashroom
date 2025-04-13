#include "application.h"

std::unique_ptr<Application> Application::app_=std::make_unique<Application>();
std::unique_ptr<Config> Application::conf_ = std::make_unique<Config>();
std::unique_ptr<LogError> Application::logger_=std::make_unique<LogError>();
//std::unique_ptr<DataInfo> Application::d_info_;
