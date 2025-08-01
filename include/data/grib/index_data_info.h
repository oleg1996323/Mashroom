#pragma once
#include "sys/error_code.h"
#include "types/time_interval.h"
#include <optional>
#include "sections/grid/grid.h"
#include "grib1/include/err.h"

struct GribIndexDataInfo
{
    std::optional<GridInfo> grid_data;
    ptrdiff_t buf_pos_;
    std::chrono::system_clock::time_point date_time = std::chrono::system_clock::time_point::max();
    ErrorCodeData err = ErrorCodeData::NONE_ERR;
};