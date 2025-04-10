#pragma once
#include <stdio.h>
#include "def.h"

typedef struct CAPDATAINFO{
    Date from;
    Date to;
    Rect rect;

}CapitalizeDataInfo;

extern CapitalizeDataInfo capitalize(const char* in,
                        const char* root_cap_dir_name,
                        const char* fmt_cap,
                        enum DATA_FORMAT output_type);

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