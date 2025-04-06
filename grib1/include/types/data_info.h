#pragma once
#include <vector>
#include <algorithm>
#include "aux_code/def.h"
#include "types/date.h"
#include "types/grid.h"
#include "types/data_suite.h"
#include "code_tables/table_0.h"
#include "sections/grid/grid.h"
#include "def.h"

STRUCT_BEG(GribDataInfo)
{
    GridInfo grid_data;
    Date from;
    Date to;
    #ifdef __cplusplus
    std::vector<uint8_t> parameters;
    #endif
    Organization center;
    uint8_t subcenter;
    #ifndef __cplusplus
    uint8_t* parameters;
    #endif
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)

    #ifdef __cplusplus
    bool contains_param(uint8_t param) const{
        return std::find(parameters.begin(),parameters.end(),param)!=parameters.end();
    }
    #endif
}
STRUCT_END(GribDataInfo)