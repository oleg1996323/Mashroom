#pragma once
#include <sys/socket.h>
#include <CLI/CLI.hpp>
#include <string>
#include <boost/lexical_cast.hpp>

namespace CLI::detail{
    template <>
        bool lexical_cast<linger>(const std::string& input, linger& output);
        template<>
        std::string to_string(const linger& input);
}

namespace boost{
    template<>
    linger lexical_cast(const std::string& input);
    template<>
    std::string lexical_cast(const linger& input);
}