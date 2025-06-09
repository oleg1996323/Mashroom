#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"

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
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};