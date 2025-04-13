#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include "aux_code/def.h"
#include "types/date.h"
#include "types/grid.h"
#include "code_tables/table_0.h"
#include "sections/grid/grid.h"
#include "def.h"

STRUCT_BEG(GribMsgDataInfo)
{
    GridInfo grid_data;
    Date date;
    ptrdiff_t buf_pos_;
    uint32_t msg_sz_;
    uint8_t parameter;
    TimeFrame t_unit;
    Organization center;
    uint8_t subcenter;
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)

    GribMsgDataInfo(GridInfo&& grid_data_,
        Date&& date_,
        uint8_t parameter_,
        ptrdiff_t msg_buf,
        uint32_t msg_size,
        TimeFrame t_unit_,
        Organization center_,
        uint8_t subcenter_):
        grid_data(std::move(grid_data_)),
        date(std::move(date_)),
        buf_pos_(msg_buf),
        msg_sz_(msg_size),
        parameter(parameter_),
        t_unit(t_unit_),
        center(center_),
        subcenter(subcenter_){}
}
STRUCT_END(GribMsgDataInfo)