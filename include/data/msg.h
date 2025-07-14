#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include "library/include/def.h"
#include "code_tables/table_0.h"
#include "code_tables/table_4.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "types/time_interval.h"
#include "grib1/include/def.h"

struct GribMsgDataInfo
{
    GridInfo grid_data;
    utc_tp date;
    ptrdiff_t buf_pos_;
    uint32_t msg_sz_;
    uint8_t parameter;
    TimeFrame t_unit;
    Organization center;
    uint8_t table_version;
    ErrorCodeData err = ErrorCodeData::NONE_ERR;

    GribMsgDataInfo(GridInfo&& grid_data_,
        utc_tp&& date_,
        ptrdiff_t msg_buf_pos,
        uint32_t msg_size,
        uint8_t parameter_,
        TimeFrame t_unit_,
        Organization center_,
        uint8_t table_version_):
        grid_data(std::move(grid_data_)),
        date(std::move(date_)),
        buf_pos_(msg_buf_pos),
        msg_sz_(msg_size),
        parameter(parameter_),
        t_unit(t_unit_),
        center(center_),
        table_version(table_version_){}
    GribMsgDataInfo() = default;
};