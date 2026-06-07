#pragma once
#include "sys/config.h"
#include <CLI/CLI.hpp>
#include "sys/application.h"

namespace parse{
    class UserConfig{
        class Add{
            CLI::App* app_;
            CLI::Option* name_;
            CLI::Option* output_files_root_dir_;
            CLI::Option* index_upd_ti_;
            CLI::Option* mashroom_upd_ti_;
            public:
            Add(CLI::App* app);
            void execute();
        };

        class Remove{
            CLI::App* app_;
            CLI::Option* name_;
            public:
            Remove(CLI::App* app);
            void execute();
        };

        class Modify{
            CLI::App* app_;
            CLI::Option* name_;
            CLI::Option* output_files_root_dir_;
            CLI::Option* index_upd_ti_;
            CLI::Option* mashroom_upd_ti_;
            public:
            Modify(CLI::App* app);
            void execute();
        };

        class Load{
            CLI::App* app_;
            CLI::Option* name_;
            CLI::Option* path_;
            public:
            Load(CLI::App* app);
            void execute();
        };

        class Print{
            CLI::App* app_;
            CLI::Option* name_;
            public:
            Print(CLI::App* app);
            void execute();
        };

        CLI::App* app_;
        CLI::App* add_;
        CLI::App* modify_;
        CLI::App* load_;
        CLI::App* remove_;
        CLI::App* print_all_;
        CLI::App* print_named_;
        CLI::App* current_;
        public:
        UserConfig(CLI::App* app);
        void execute();
    };
}