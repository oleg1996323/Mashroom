#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"

#ifdef __cplusplus
#include <span>
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
    
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
};
#endif