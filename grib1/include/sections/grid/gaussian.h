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

    GridDefinition(unsigned char* buffer):
        nx(GDS_LatLon_nx(buffer)),
        ny(GDS_LatLon_ny(buffer)),
        y1(0.001*GDS_LatLon_La1(buffer)),
        x1(0.001*GDS_LatLon_Lo1(buffer)),
        directionIncrement(GDS_LatLon_dx(buffer)),
        N(GDS_Gaussian_nlat(buffer)),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))),
        y2(0.001*GDS_LatLon_La2(buffer)),
        x2(0.001*GDS_LatLon_Lo2(buffer)),
        scan_mode(ScanMode(GDS_LatLon_scan(buffer))){}

    bool operator==(const GridDefinition<RepresentationType::GAUSSIAN>& other) const{
        return y1 == other.y1 &&
        x1 == other.x1 &&
        y2 == other.y2 &&
        x2 == other.x2 &&
        ny == other.ny &&
        nx == other.nx &&
        scan_mode == other.scan_mode &&
        resolutionAndComponentFlags == other.resolutionAndComponentFlags &&
        N == other.N &&
        directionIncrement == other.directionIncrement;
    }
};

#include <cassert>
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

    GridDefinition(unsigned char* buffer):
        nx(GDS_LatLon_nx(buffer)),
        ny(GDS_LatLon_ny(buffer)),
        y1(0.001*GDS_LatLon_La1(buffer)),
        x1(0.001*GDS_LatLon_Lo1(buffer)),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))),
        y2(0.001*GDS_LatLon_La2(buffer)),
        x2(0.001*GDS_LatLon_Lo2(buffer)),
        dy(0.001*GDS_LatLon_dy(buffer)),
        dx(0.001*GDS_LatLon_dx(buffer)),
        scan_mode(ScanMode(GDS_LatLon_mode(buffer))){}

    bool operator==(const GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>& other) const{
        assert(false);
        return false;
    }
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

    GridDefinition(unsigned char* buffer):
        nx(GDS_LatLon_nx(buffer)),
        ny(GDS_LatLon_ny(buffer)),
        y1(0.001*GDS_LatLon_La1(buffer)),
        x1(0.001*GDS_LatLon_Lo1(buffer)),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))),
        y2(0.001*GDS_LatLon_La2(buffer)),
        x2(0.001*GDS_LatLon_Lo2(buffer)),
        dy(0.001*GDS_LatLon_dy(buffer)),
        dx(0.001*GDS_LatLon_dx(buffer)),
        scan_mode(ScanMode(GDS_LatLon_mode(buffer))){}
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

    GridDefinition(unsigned char* buffer):
        nx(GDS_LatLon_nx(buffer)),
        ny(GDS_LatLon_ny(buffer)),
        y1(0.001*GDS_LatLon_La1(buffer)),
        x1(0.001*GDS_LatLon_Lo1(buffer)),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))),
        y2(0.001*GDS_LatLon_La2(buffer)),
        x2(0.001*GDS_LatLon_Lo2(buffer)),
        dy(0.001*GDS_LatLon_dy(buffer)),
        dx(0.001*GDS_LatLon_dx(buffer)),
        scan_mode(ScanMode(GDS_LatLon_mode(buffer))){}
};
#endif