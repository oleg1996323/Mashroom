#pragma once
#include <CLI/CLI.hpp>
#include "definitions/path_process.h"
#include "types_parse/information_parse.h"
#include "proc/index/indexoutputfileformat.h"
#include <vector>
#include <thread>

namespace parse{
    class Index{
        CLI::App* app_;
        CLI::Option* jobs_;
        CLI::Option* output_dir_;
        CLI::Option* input_paths_;
        CLI::Option* format_;
        CLI::Option* overwrite_;
        CLI::Option* ref_;
        CLI::Option* web_file_max_size_;
        std::string output_dir_val_;
        std::vector<path::Storage<false>> input_paths_val_;
        IndexOutputFileFormat::token fmt_val_;
        int32_t jobs_val_=std::thread::hardware_concurrency();
        std::string info_val_;
        public:
        Index(CLI::App* app);
        void execute();
    };
}