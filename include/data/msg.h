#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include "aux_code/def.h"
#include "code_tables/table_0.h"
#include "sections/grid/grid.h"
#include "def.h"

STRUCT_BEG(GribMsgDataInfo)
{
    std::optional<GridInfo> grid_data;
    utc_tp date;
    ptrdiff_t buf_pos_;
    uint32_t msg_sz_;
    uint8_t parameter;
    TimeFrame t_unit;
    Organization center;
    uint8_t table_version;
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)

    GribMsgDataInfo(std::optional<GridInfo>&& grid_data_,
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
}
STRUCT_END(GribMsgDataInfo)