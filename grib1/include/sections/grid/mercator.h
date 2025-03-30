#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "float.h"

#ifdef __cplusplus
#include <span>
template<RepresentationType>
struct GridDefinition;

template<>
struct GridDefinition<RepresentationType::MERCATOR>{
    float y1;
    float x1;
    float y2;
    float x2;
    float dy;
    float dx;
    uint16_t ny;
    uint16_t nx;
    uint16_t latin;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;

    GridDefinition(unsigned char* buffer){
        nx = GDS_Merc_nx(buffer);
        ny = GDS_Merc_ny(buffer);
        y1 = GDS_Merc_La1(buffer);
        x1 = GDS_Merc_Lo1(buffer);
        resolutionAndComponentFlags = (ResolutionComponentFlags)GDS_Merc_mode(buffer);
        y2 = GDS_Merc_La2(buffer);
        x2 = GDS_Merc_Lo2(buffer);
        dy = GDS_Merc_dy(buffer);
        dx = GDS_Merc_dx(buffer);
        scan_mode = (ScanMode)GDS_Merc_mode(buffer);
        latin = GDS_Merc_Latin(buffer);
    }
};
#endif