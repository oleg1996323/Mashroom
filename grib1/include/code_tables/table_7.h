#pragma once

#pragma pack(push,1)
#ifdef __cplusplus
    struct ResolutionComponentFlags
    {
        unsigned given_direction:1;
        unsigned earth_spheroidal:1;
        unsigned :2;
        unsigned grid_direction_uv_comp:1;
        unsigned :3;
    };
#else
    typedef struct
    {
        unsigned given_direction:1;
        unsigned earth_spheroidal:1;
        unsigned :2;
        unsigned grid_direction_uv_comp:1;
        unsigned :3;
    }ResolutionComponentFlags;
#endif
#pragma pack(pop)