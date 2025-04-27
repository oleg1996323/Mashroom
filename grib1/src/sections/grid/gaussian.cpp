#pragma once
#include <sections/grid/gaussian.h>
#include <cassert>

GridDefinition<RepresentationType::GAUSSIAN>::GridDefinition(unsigned char* buffer):
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

bool GridDefinition<RepresentationType::GAUSSIAN>::operator==(const GridDefinition<RepresentationType::GAUSSIAN>& other) const{
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
const char* GridDefinition<RepresentationType::GAUSSIAN>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
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

bool GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::operator==(const GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>& other) const{
    assert(false);
    return false;
}
const char* GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
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
const char* GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
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
const char* GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}