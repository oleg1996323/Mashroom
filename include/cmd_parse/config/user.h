#pragma once
#include "sys/config.h"
#include <CLI/CLI.hpp>
#include "sys/application.h"

namespace parse{
    class UserConfig{
        class Add{
            CLI::App* app_;
            std::string name_val_;
            std::string output_files_root_dir_val_;
            DateTimeDiff index_upd_ti_val_;
            DateTimeDiff mashroom_upd_ti_val_;
            public:
            Add(CLI::App* app);
            void execute();
        };

        class Remove{
            CLI::App* app_;
            std::string name_val_;
            public:
            Remove(CLI::App* app);
            void execute();
        };

        class Modify{
            CLI::App* app_;
            std::string name_val_;
            std::string output_files_root_dir_val_;
            DateTimeDiff index_upd_ti_val_;
            DateTimeDiff mashroom_upd_ti_val_;
            public:
            Modify(CLI::App* app);
            void execute();
        };

        class Load{
            CLI::App* app_;
            std::string name_val_;
            std::string path_val_;
            public:
            Load(CLI::App* app);
            void execute();
        };

        class Print{
            CLI::App* app_;
            std::string name_val_;
            public:
            Print(CLI::App* app);
            void execute();
        };

        CLI::App* app_;
        public:
        UserConfig(CLI::App* app);
        void execute();
    };
}