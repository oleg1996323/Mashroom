#pragma once
#include <sys/time.h>
#include <CLI/CLI.hpp>
#include <string>
#include <boost/lexical_cast.hpp>

namespace boost{
    template<>
    timeval lexical_cast(const std::string& input);
    template<>
    std::string lexical_cast(const timeval& input);
}

namespace CLI::detail{
    template <>
    bool lexical_cast<timeval>(const std::string& input, timeval& output);
    template<>
    std::string to_string(const timeval& input);
}