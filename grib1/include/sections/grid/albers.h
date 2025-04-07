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
struct GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>{
    float y1;
    float x1;
    float LoV;
    float latin1;
    float latin2;
    float Dy;
    float Dx;
    int32_t latitude_south_pole;
    int32_t longitude_south_pole;
    uint16_t ny;
    uint16_t nx;
    bool is_south_pole;
    bool is_bipolar;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;
    
    GridDefinition(unsigned char* buffer):
        nx(GDS_Lambert_nx(buffer)),
        ny(GDS_Lambert_ny(buffer)),
        y1(0.001*GDS_Lambert_La1(buffer)),
        x1(0.001*GDS_Lambert_Lo1(buffer)),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Lambert_mode(buffer))),
        LoV(0.001*GDS_Lambert_Lov(buffer)),
        Dy(0.001*GDS_Lambert_dy(buffer)),
        Dx(0.001*GDS_Lambert_dx(buffer)),
        is_south_pole((GDS_Lambert_NP(buffer)&0b01)!=0),
        is_bipolar((GDS_Lambert_NP(buffer)&0b10)!=0),
        scan_mode(ScanMode(GDS_Lambert_scan(buffer))),
        latin1(0.001*GDS_Lambert_Latin1(buffer)),
        latin2(0.001*GDS_Lambert_Latin2(buffer)),
        latitude_south_pole(0.001*GDS_Lambert_LatSP(buffer)),
        longitude_south_pole(0.001*GDS_Lambert_LonSP(buffer)){}
};
#endif