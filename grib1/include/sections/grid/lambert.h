#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <span>
template<>
struct GridDefinition<RepresentationType::LAMBERT>{
    float y1;
    float x1;
    float dy;
    float dx;
    float LoV;
    float latin1;
    float latin2;
    int latitude_south_pole;
    int longitude_south_pole;
    uint16_t ny;
    uint16_t nx;
    bool is_south_pole;
    bool is_bipolar;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
};

template<>
struct GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
};
