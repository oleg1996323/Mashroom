#include <sections/grid/albers.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::GridDefinition(unsigned char* buffer):
    nx(GDS_Lambert_nx(buffer)),
    ny(GDS_Lambert_ny(buffer)),
    y1(0.001*GDS_Lambert_La1(buffer)),
    x1(0.001*GDS_Lambert_Lo1(buffer)),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Lambert_mode(buffer))),
    LoV(0.001*GDS_Lambert_Lov(buffer)),
    Dy(0.001*GDS_Lambert_dy(buffer)),
    Dx(0.001*GDS_Lambert_dx(buffer)),
    is_south_pole((GDS_Lambert_NP(buffer)&0b01)!=0),
    is_bipolar((GDS_Lambert_NP(buffer)&0b10)!=0),
    scan_mode(ScanMode(GDS_Lambert_scan(buffer))),
    latin1(0.001*GDS_Lambert_Latin1(buffer)),
    latin2(0.001*GDS_Lambert_Latin2(buffer)),
    latitude_south_pole(0.001*GDS_Lambert_LatSP(buffer)),
    longitude_south_pole(0.001*GDS_Lambert_LonSP(buffer)){}

const char* GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::print_grid_info() const{
    assert(false);//still not available
}