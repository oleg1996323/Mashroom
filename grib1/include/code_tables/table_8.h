#pragma once

#pragma pack(push,1)
#ifdef __cplusplus
    struct ScanMode
    {
        unsigned points_sub_i_dir:1;
        unsigned points_sub_j_dir:1;
        unsigned adj_points_j_dir:1;
    };
#else
    typedef struct
    {
        unsigned points_sub_i_dir:1;
        unsigned points_sub_j_dir:1;
        unsigned adj_points_j_dir:1;
    }ScanMode;
#endif
#pragma pack(pop)