#pragma once
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/log_err.h"
#include <set>
#include <CLI/CLI.hpp>

namespace parse{
    class Network{
        CLI::App* app_;
        uint16_t timeout_ = 0;
        public:
        Network(CLI::App* app);
        void execute();
    };
}