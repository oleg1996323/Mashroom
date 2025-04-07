#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"

#ifdef __cplusplus
#include <span>
template<RepresentationType>
struct GridDefinition;

template<>
struct GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>{
    float y1;
    float x1;
    float y2;
    float x2;
    float LoV;
    uint32_t Dy;
    uint32_t Dx;
    uint16_t ny;
    int16_t nx;
    uint16_t directionIncrement;
    uint16_t N;
    bool is_south_pole;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;
    
    GridDefinition(unsigned char* buffer):
        nx(GDS_Polar_nx(buffer)),
        ny(GDS_Polar_ny(buffer)),
        y1(0.001*GDS_Polar_La1(buffer)),
        x1(0.001*GDS_Polar_Lo1(buffer)),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Polar_mode(buffer))),
        LoV(0.001*GDS_Polar_Lov(buffer)),
        Dy(GDS_Polar_Dy(buffer)),
        Dx(GDS_Polar_Dx(buffer)),
        is_south_pole(GDS_Polar_pole(buffer)),
        scan_mode(ScanMode(GDS_Polar_mode(buffer))){}
};
#endif