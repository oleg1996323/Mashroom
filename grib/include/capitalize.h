#pragma once
#include <stdio.h>
#include "def.h"

typedef struct ORDER_ITEMS{
    int hour;
    int day;
    int month;
    int year;
    int lat;
    int lon;
    DataFormat fmt;
}OrderItems;

#define OrderItems(...) ((OrderItems){.hour=-1,.day=-1,.month=-1,.year=-1,.lat=-1,.lon=-1,.fmt = NONE},##__VA_ARGS__)

typedef struct CAPITALIZEDATA{
    Date from;
    Date to;
    GridData grid_data;
    OrderItems order;
    ErrorCodeData err;
}CapitalizeData;

#define CapitalizeData(...) ((CapitalizeData){.from = Date(), .to = Date(),.grid_data = GridData(),.order = OrderItems(),.err = NONE_ERR})

extern CapitalizeData capitalize(const char* in,
    const char* root_cap_dir_name,
    const char* fmt_cap,
    enum DATA_FORMAT output_type);