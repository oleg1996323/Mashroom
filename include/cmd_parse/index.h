#pragma once
#include <CLI/CLI.hpp>
#include "Location.h"
#include "types_parse/information_parse.h"
#include "proc/index/indexoutputfileformat.h"
#include <vector>
#include <thread>

namespace parse{
    class Index{
        CLI::App* app_;
        std::string output_dir_val_;
        std::vector<Location<false>> input_paths_val_;
        IndexOutputFileFormat::token fmt_val_;
        int32_t jobs_val_=std::thread::hardware_concurrency();
        std::string info_val_;
        public:
        Index(CLI::App* app);
        void execute();
    };
}