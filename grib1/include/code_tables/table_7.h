#pragma once
#include "aux_code/def.h"
#pragma pack(push,1)
STRUCT_BEG(ResolutionComponentFlags)
{
    unsigned given_direction:1;
    unsigned earth_spheroidal:1;
    unsigned :2;
    unsigned grid_direction_uv_comp:1;
    unsigned :3;
}
STRUCT_END(ResolutionComponentFlags)
#pragma pack(pop)