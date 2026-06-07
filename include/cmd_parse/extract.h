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
        CLI::Option* jobs_;
        CLI::Option* output_dir_;
        CLI::Option* input_paths_;
        CLI::Option* from_;
        CLI::Option* to_;
        CLI::Option* coord_;
        CLI::Option* centers_;
        CLI::Option* parameters_;
        CLI::Option* collections_;
        CLI::Option* time_forecast_;
        CLI::Option* level_;
        CLI::Option* grid_;
        CLI::Option* output_fmt_;
        CLI::Option* time_interval_;

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
        DateTimeDiff time_interval_val_;
        ::OutputDataFileFormats output_fmt_val_;
        public:
        Extract(CLI::App* app);
        void execute();
    };
}