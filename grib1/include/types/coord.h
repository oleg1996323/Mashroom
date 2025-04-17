#pragma once
#include "aux_code/def.h"
typedef float Lat;
typedef float Lon;

STRUCT_BEG(Coord)
{
    Lat lat_ DEF_STRUCT_VAL(-999);
    Lon lon_ DEF_STRUCT_VAL(-999);
}
STRUCT_END(Coord)

STRUCT_BEG(RawCoord)
{
    uint32_t lat_ DEF_STRUCT_VAL(-999);
    uint32_t lon_ DEF_STRUCT_VAL(-999);
}
STRUCT_END(RawCoord)

#ifndef __cplusplus
#define Coord(...) ((Coord) { .lat_ = -999, .lon_ = -999},##__VA_ARGS__)
#endif

extern bool is_correct_pos(const Coord* pos);