#pragma once
#include "aux_code/def.h"

STRUCT_BEG(DataSuite)
{
    uint8_t discipline;
    uint8_t category;
    uint8_t parameter;
    uint8_t grib_v;
}
STRUCT_END(DataSuite)