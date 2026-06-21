#pragma once
#include <CLI/CLI.hpp>
#include "network/address.h"
#include <regex>
#include <string>

namespace CLI {
namespace detail {
    template <>
    bool lexical_cast(const std::string& input, ::network::Address& output);    
}
}

std::ostream& operator<<(std::ostream& stream,const network::Address address);
std::istream& operator>>(std::istream& stream,network::Address& address);