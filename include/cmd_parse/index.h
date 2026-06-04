#pragma once
#include <CLI/CLI.hpp>
#include "proc/index.h"

namespace parse{
    class Index{
        CLI::App* app_;
        CLI::App * index_=app_->add_subcommand("index",
                "Read specified files and register\
                the contained data properties and\
                data positions. Organize the data\
                by defined hierarchy from unique\
                massive archive.")->require_subcommand(1)->
                alias("-I");
        std::unique_ptr<::Index> hIndex;
        CLI::Option* jobs_;
        CLI::Option* output_dir_;
        CLI::Option* input_paths_;
        CLI::Option* format_;
        CLI::Option* overwrite_;
        CLI::Option* ref_;
        CLI::Option* web_file_max_size_;
        Index(CLI::App* app);

        void execute();
    };
}