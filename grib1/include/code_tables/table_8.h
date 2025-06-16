#pragma once
#include "functional/def.h"
STRUCT_BEG(ScanMode)
{
    bool points_sub_i_dir;
    bool points_sub_j_dir;
    bool adj_points_j_dir;

    #ifdef __cplusplus
    ScanMode() = default;
    ScanMode(unsigned char buf):
    points_sub_i_dir(buf&0b00000001),
    points_sub_j_dir(buf&0b00000010),
    adj_points_j_dir(buf&0b00000100){}
    #endif
    bool operator==(const ScanMode& other) const{
        return  points_sub_i_dir == other.points_sub_i_dir &&
                points_sub_j_dir == other.points_sub_j_dir &&
                adj_points_j_dir == other.adj_points_j_dir;
    }
}
STRUCT_END(ScanMode)