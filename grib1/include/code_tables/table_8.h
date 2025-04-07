#pragma once
#include "aux_code/def.h"
STRUCT_BEG(ScanMode)
{
    bool points_sub_i_dir;
    bool points_sub_j_dir;
    bool adj_points_j_dir;

    #ifdef __cplusplus
    ScanMode(unsigned char buf):
    points_sub_i_dir(buf&0b00000001),
    points_sub_j_dir(buf&0b00000010),
    adj_points_j_dir(buf&0b00000100){}
    #endif
}
STRUCT_END(ScanMode)