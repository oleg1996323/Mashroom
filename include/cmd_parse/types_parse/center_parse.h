#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error.h"
#include <thread>
#include "grib1/code_tables.h"
#include "proc/interfaces/abstractsearchprocess.h"
#include "OsterLib/types/time_interval.h"
#include "grid_type_parse.h"
#include "grib1/sections.h"
#include "param_tablev_parse.h"
#include "coord_parse.h"
#include <expected>

namespace parse{
    std::expected<Organization,osterlib::ContextedError> center_notifier(const std::vector<std::string>& input) noexcept;
}