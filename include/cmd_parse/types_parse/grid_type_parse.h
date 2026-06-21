#pragma once
#include "code_tables/table_6.h"
#include <vector>
#include <string>
#include <expected>
#include "sys/error_code.h"
#include <boost/lexical_cast.hpp>

namespace boost{
template<>
RepresentationType lexical_cast(const std::string& input);
}

namespace parse{
    std::expected<RepresentationType,ErrorCode> grid_notifier(const std::vector<std::string>& input) noexcept;
}