#pragma once
#include "aux_code/def.h"
STRUCT_BEG(ResolutionComponentFlags)
{
    bool given_direction;
    bool earth_spheroidal;
    bool grid_direction_uv_comp;
    #ifdef __cplusplus
    ResolutionComponentFlags(unsigned char buf):
    given_direction(buf&0b00000001),
    earth_spheroidal(buf&0b00000010),
    grid_direction_uv_comp(buf&0b00010000){}
    #endif
}
STRUCT_END(ResolutionComponentFlags)