#include "log_err.h"
#include <sys/application.h>
std::unique_ptr<LogError> Application::logger_=std::make_unique<LogError>();