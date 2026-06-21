#pragma once
#include <CLI/CLI.hpp>

namespace parse{
    class Mashroom{
        CLI::App* app_;
        public:
        Mashroom(CLI::App& app);
        void execute();
        void print_help(std::ostream& os) const noexcept;
    };
}