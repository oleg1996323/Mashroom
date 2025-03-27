#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"

#ifdef __cplusplus
#pragma pack(push,1)
template<RepresentationType>
struct GridDefinition;

template<>
struct GridDefinition<RepresentationType::UTM>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1:24;
    uint32_t x1:24;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2:24;
    uint32_t x2:24;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
};
#pragma pack(pop)
#endif