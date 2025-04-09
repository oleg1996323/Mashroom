#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "sections/grid/def.h"
#include "types/coord.h"
#include "grib1/include/def.h"

#ifdef __cplusplus
#include <span>
template<RepresentationType>
struct GridDefinition;

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

    GridDefinition(unsigned char* buffer):
        nx(GDS_LatLon_nx(buffer)),
        ny(GDS_LatLon_ny(buffer)),
        y1(GDS_LatLon_La1(buffer)),
        x1(GDS_LatLon_Lo1(buffer)),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))),
        y2(GDS_LatLon_La2(buffer)),
        x2(GDS_LatLon_Lo2(buffer)),
        dy(GDS_LatLon_dy(buffer)),
        dx(GDS_LatLon_dx(buffer)),
        scan_mode(ScanMode(GDS_LatLon_scan(buffer))){}

    bool operator==(const GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>& other) {
        return y1 == other.y1 &&
        x1 == other.x1 &&
        y2 == other.y2 &&
        x2 == other.x2 &&
        dy == other.dy &&
        dx == other.dx &&
        ny == other.ny &&
        nx == other.nx &&
        scan_mode == other.scan_mode &&
        resolutionAndComponentFlags == other.resolutionAndComponentFlags;
    }
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

    GridDefinition(unsigned char* buffer):
        nx(GDS_RotLL_nx(buffer)),
        ny(GDS_RotLL_ny(buffer)),
        y1(GDS_RotLL_La1(buffer)),
        x1(GDS_RotLL_Lo1(buffer)),
        resolutionAndComponentFlags(ResolutionComponentFlags(GDS_RotLL_mode(buffer))),
        y2(GDS_RotLL_La2(buffer)),
        x2(GDS_RotLL_Lo2(buffer)),
        dy(GDS_RotLL_dy(buffer)),
        dx(GDS_RotLL_dx(buffer)),
        scan_mode(ScanMode(GDS_RotLL_scan(buffer))), //TODO: correct (see above)
        angle_rotation(GDS_RotLL_RotAng(buffer)),
        latitude_south_pole(GDS_RotLL_LaSP(buffer)),
        longitude_south_pole(GDS_RotLL_LoSP(buffer)){}
    bool operator==(const GridDefinition<RepresentationType::ROTATED_LAT_LON>& other) {
        return y1 == other.y1 &&
        x1 == other.x1 &&
        y2 == other.y2 &&
        x2 == other.x2 &&
        dy == other.dy &&
        dx == other.dx &&
        ny == other.ny &&
        nx == other.nx &&
        scan_mode == other.scan_mode &&
        resolutionAndComponentFlags == other.resolutionAndComponentFlags &&
        latitude_south_pole == other.latitude_south_pole &&
        longitude_south_pole == other.longitude_south_pole && 
        angle_rotation == other.angle_rotation;
    }
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

    GridDefinition(unsigned char* buffer):
        nx(GDS_StrLL_nx(buffer)),
        ny(GDS_StrLL_ny(buffer)),
        y1(GDS_StrLL_La1(buffer)),
        x1(GDS_StrLL_Lo1(buffer)),
        resolutionAndComponentFlags((ResolutionComponentFlags)GDS_StrLL_mode(buffer)),
        y2(GDS_StrLL_La2(buffer)),
        x2(GDS_StrLL_Lo2(buffer)),
        dy(GDS_StrLL_dy(buffer)),
        dx(GDS_StrLL_dx(buffer)),
        scan_mode(ScanMode(GDS_StrLL_scan(buffer))),
        stretch_factor(GDS_StrLL_StrFactor(buffer)),
        latitude_stretch_pole(GDS_StrLL_LaStrP(buffer)),
        longitude_stretch_pole(GDS_StrLL_LoStrP(buffer)){}

    bool operator==(const GridDefinition<RepresentationType::STRETCHED_LAT_LON>& other){
        return y1 == other.y1 &&
        x1 == other.x1 &&
        y2 == other.y2 &&
        x2 == other.x2 &&
        dy == other.dy &&
        dx == other.dx &&
        ny == other.ny &&
        nx == other.nx &&
        scan_mode == other.scan_mode &&
        resolutionAndComponentFlags == other.resolutionAndComponentFlags &&
        latitude_stretch_pole == other.latitude_stretch_pole &&
        longitude_stretch_pole == other.longitude_stretch_pole && 
        stretch_factor == other.stretch_factor;
    }
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

    GridDefinition(unsigned char* buffer):
        nx(GDS_StrLL_nx(buffer)),
        ny(GDS_StrLL_ny(buffer)),
        y1(GDS_StrLL_La1(buffer)),
        x1(GDS_StrLL_Lo1(buffer)),
        resolutionAndComponentFlags((ResolutionComponentFlags)GDS_StrLL_mode(buffer)),
        y2(GDS_StrLL_La2(buffer)),
        x2(GDS_StrLL_Lo2(buffer)),
        dy(GDS_StrLL_dy(buffer)),
        dx(GDS_StrLL_dx(buffer)),
        scan_mode(ScanMode(GDS_StrLL_mode(buffer))),
        stretch_factor(GDS_StrLL_StrFactor(buffer)),
        latitude_south_pole(GDS_RotLL_LaSP(buffer)),
        longitude_south_pole(GDS_RotLL_LoSP(buffer)),
        latitude_stretch_pole(GDS_StrLL_LaStrP((&buffer[43-33]))),
        longitude_stretch_pole(GDS_StrLL_LoStrP((&buffer[43-33]))){}

    bool operator==(const GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>& other){
        return y1 == other.y1 &&
        x1 == other.x1 &&
        y2 == other.y2 &&
        x2 == other.x2 &&
        dy == other.dy &&
        dx == other.dx &&
        ny == other.ny &&
        nx == other.nx &&
        scan_mode == other.scan_mode &&
        resolutionAndComponentFlags == other.resolutionAndComponentFlags &&
        latitude_stretch_pole == other.latitude_stretch_pole &&
        longitude_stretch_pole == other.longitude_stretch_pole && 
        stretch_factor == other.stretch_factor &&
        latitude_stretch_pole == other.latitude_stretch_pole &&
        longitude_stretch_pole == other.longitude_stretch_pole && 
        stretch_factor == other.stretch_factor;
    }
};
#endif