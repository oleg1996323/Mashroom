#pragma once
#include "types/coord.h"
#include <vector>
#include <CLI/CLI.hpp>

namespace CLI {
    namespace detail {
        template <>
        bool lexical_cast<Coord>(const std::string& input, Coord& output);
        template<>
        std::string to_string(const Coord& val);
    }
}