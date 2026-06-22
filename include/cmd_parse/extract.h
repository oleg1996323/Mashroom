#pragma once
#include <CLI/CLI.hpp>
#include <vector>
#include <thread>
#include "sys/application.h"
#include "types/time_interval.h"
#include "types/coord.h"
#include "types_parse/center_parse.h"
#include "types_parse/coord_parse.h"

namespace parse{
    class Extract{
        CLI::App* app_;

        std::string output_dir_val_;
        std::vector<std::string> input_paths_val_;
        int32_t jobs_val_=std::thread::hardware_concurrency();
        std::string from_val_;
        std::string to_val_;
        Coord pos_val_;
        std::vector<std::string> centers_val_;
        std::vector<std::string> parameters_val_;
        std::vector<std::string> collections_val_;
        std::string time_forecast_val_;
        std::string level_val_;
        std::vector<std::string> grid_val_;
        uint32_t years_ = 0;
        uint32_t months_ = 0;
        uint32_t days_ = 0;
        uint32_t hours_ = 0;
        uint32_t minutes_ = 0;
        uint32_t seconds_ = 0;
        ::OutputDataFileFormats output_fmt_val_;
        public:
        Extract(CLI::App* app);
        void execute();
    };
}