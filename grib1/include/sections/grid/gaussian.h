#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <span>
template<>
struct GridDefinition<RepresentationType::GAUSSIAN>{
    float y1;
    float x1;
    float y2;
    float x2;
    uint16_t ny;
    int16_t nx;
    uint16_t directionIncrement;
    uint16_t N;
    ScanMode scan_mode;
    ResolutionComponentFlags resolutionAndComponentFlags;

    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::GAUSSIAN>& other) const;
    void serialize(std::vector<char>& buf) const{
        
    }
    void deserialize(const std::vector<char>& buf);
};
template<>
struct GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>{
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
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>& other) const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};
template<>
struct GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>{
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
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};
template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>{
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
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};