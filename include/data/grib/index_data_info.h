#pragma once
#include "sys/error_code.h"
#include "types/time_interval.h"
#include <optional>
#include "sections/grid/grid.h"
#include "err.h"
#include "common/api_types.h"
#include "grib1/error_data.h"

template<API::TYPES TYPE>
struct IndexDataInfo;

template<>
struct IndexDataInfo<API::GRIB1>
{
    std::optional<GridInfo> grid_data;
    ptrdiff_t buf_pos_;
    std::chrono::system_clock::time_point date_time = std::chrono::system_clock::time_point::max();
    API::ErrorData::Code<API::TYPES::GRIB1>::value err = API::ErrorData::Code<API::TYPES::GRIB1>::value::NONE_ERR;

    bool operator==(const IndexDataInfo& other) const{
        return grid_data==other.grid_data && buf_pos_==other.buf_pos_ && date_time==other.date_time &&
        err == other.err;
    }
};