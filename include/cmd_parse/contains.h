#pragma once

#include <CLI/CLI.hpp>
#include "Location.h"
#include "types_parse/information_parse.h"
#include <vector>
#include <thread>

namespace parse{
    class Contains{
        CLI::App* app_;
        CLI::Option* jobs_;
        int32_t jobs_val_=std::thread::hardware_concurrency();
        public:
        Contains(CLI::App* app);
        void execute();
    };
}