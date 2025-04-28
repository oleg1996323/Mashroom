#pragma once
#include "code_tables/table_6.h"
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "types/coord.h"
#include "def.h"

#include <span>

template<>
struct GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>{
    float y1;
    float x1;
    float y2;
    float x2;
    float dy;
    float dx;
    uint16_t ny;
    uint16_t nx;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;

    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>& other) const;
    const char* print_grid_info() const;
};

#include "aux_code/ibmtofloat.h"
template<>
struct GridDefinition<RepresentationType::ROTATED_LAT_LON>{
    double angle_rotation;
    float y1;
    float x1;
    float y2;
    float x2;
    float dy;
    float dx;
    int latitude_south_pole;
    int longitude_south_pole;
    uint16_t ny;
    uint16_t nx;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;

    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::ROTATED_LAT_LON>& other) const;
    const char* print_grid_info() const;
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_LAT_LON>{
    double stretch_factor;
    float y1;
    float x1;
    float y2;
    float x2;
    float dy;
    float dx;
    int latitude_stretch_pole;
    int longitude_stretch_pole;
    uint16_t ny;
    uint16_t nx;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;

    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_LAT_LON>& other) const;
    const char* print_grid_info() const;
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>{
    double angle_rotation;
    double stretch_factor;
    float y1;
    float x1;
    float y2;
    float x2;
    float dy;
    float dx;
    int latitude_south_pole;
    int longitude_south_pole;
    int latitude_stretch_pole;
    int longitude_stretch_pole;
    uint16_t ny;
    uint16_t nx;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;

    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>& other) const;
    const char* print_grid_info() const;
};

