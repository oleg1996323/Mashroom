#pragma once
#include <stdio.h>
#include "def.h"
#include "data/data_tags.h"

typedef struct CAPITALIZEITEMDATA{
    Date from;
    Date to;
    GridData grid_data;
    Item data_info;
}CapitalizeItemData;

typedef struct CAPITALIZEDATA{
    Date from;
    Date to;
    GridData grid_data;
    unsigned long sections;
    ErrorCodeData err;
}CapitalizeData;

#define CapitalizeData(...) ((CapitalizeData){.from = Date(), .to = Date(),.grid_data = GridData(),.sections=0,.err = NONE_ERR})

extern CapitalizeData capitalize(const char* in,
    const char* root_cap_dir_name,
    const char* fmt_cap,
    enum DATA_FORMAT output_type);