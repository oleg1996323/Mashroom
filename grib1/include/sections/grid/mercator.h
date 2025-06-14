#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "float.h"
#include <span>

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

    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};