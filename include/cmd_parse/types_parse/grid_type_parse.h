#pragma once
#include "grib1/code_tables.h"
#include <vector>
#include <string>
#include <expected>
#include "sys/error.h"
#include <boost/lexical_cast.hpp>

namespace boost{
template<>
RepresentationType lexical_cast(const std::string& input);
}

namespace parse{
    std::expected<RepresentationType,mashroom::errc> grid_notifier(const std::vector<std::string>& input) noexcept;
}