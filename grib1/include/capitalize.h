#pragma once
#include <stdio.h>
#include "def.h"
#include "types/date.h"
#include "types/grid.h"
#include "aux_code/def.h"

STRUCT_BEG(CapitalizeItemData)
{
    Date from;
    Date to;
    GridData grid_data;
    Item data_info;
}
STRUCT_END(CapitalizeItemData)

STRUCT_BEG(CapitalizeData)
{
    Date from DEF_STRUCT_VAL(Date())
    Date to DEF_STRUCT_VAL(Date())
    GridData grid_data DEF_STRUCT_VAL(GridData())
    unsigned long sections DEF_STRUCT_VAL(0)
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)
}
STRUCT_END(CapitalizeData)

#ifndef __cplusplus
#define CapitalizeData(...) ((CapitalizeData){.from = Date(), .to = Date(),.grid_data = GridData(),.sections=0,.err = NONE_ERR})
#endif

extern CapitalizeData capitalize(const char* in,
    const char* root_cap_dir_name,
    const char* fmt_cap,
    enum DATA_FORMAT output_type);