#pragma once
#include "byte_order.h"
struct ResolutionComponentFlags
{
    bool given_direction;
    bool earth_spheroidal;
    bool grid_direction_uv_comp;
    #ifdef __cplusplus
    ResolutionComponentFlags() = default;
    ResolutionComponentFlags(unsigned char buf):
    given_direction(buf&0b00000001),
    earth_spheroidal(buf&0b00000010),
    grid_direction_uv_comp(buf&0b00010000){}
    #endif
    bool operator==(const ResolutionComponentFlags& other) const{
        return  given_direction == other.given_direction &&
                earth_spheroidal == other.earth_spheroidal &&
                grid_direction_uv_comp == other.grid_direction_uv_comp;
    }
};