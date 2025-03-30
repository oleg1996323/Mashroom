#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"

#ifdef __cplusplus
#include <span>
#include <stdexcept>
template<RepresentationType>
struct GridDefinition;

template<>
struct GridDefinition<RepresentationType::MILLERS_CYLINDR>{
    uint16_t ny;
    uint16_t nx;
    uint32_t y1;
    uint32_t x1;
    ResolutionComponentFlags resolutionAndComponentFlags;
    uint32_t y2;
    uint32_t x2;
    uint16_t dy;
    uint16_t dx;
    ScanMode scan_mode;
    uint8_t reserved[3];
    GridDefinition(unsigned char* buffer){
        throw std::invalid_argument("Unable to use Millers coordinate-system");
    }
};
#endif