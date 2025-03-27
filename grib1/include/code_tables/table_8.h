#pragma once
#include "aux_code/def.h"
#pragma pack(push,1)
STRUCT_BEG(ScanMode)
{
    unsigned points_sub_i_dir:1;
    unsigned points_sub_j_dir:1;
    unsigned adj_points_j_dir:1;
}
STRUCT_END(ScanMode)
#pragma pack(pop)