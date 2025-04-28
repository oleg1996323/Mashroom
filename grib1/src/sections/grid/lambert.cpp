#include <sections/grid/lambert.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>

GridDefinition<RepresentationType::LAMBERT>::GridDefinition(unsigned char* buffer):
    nx(GDS_Lambert_nx(buffer)),
    ny(GDS_Lambert_ny(buffer)),
    y1(0.001*GDS_Lambert_La1(buffer)),
    x1(0.001*GDS_Lambert_Lo1(buffer)),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Lambert_mode(buffer))),
    LoV(GDS_Lambert_Lov(buffer)),
    dy(0.001*GDS_Lambert_dy(buffer)),
    dx(0.001*GDS_Lambert_dx(buffer)),
    is_south_pole((buffer[26]&0b10000000)==0), //GDS_Lambert_NP
    is_bipolar((buffer[26]&0b01000000)==1),
    scan_mode(ScanMode(GDS_Lambert_scan(buffer))),
    latin1(GDS_Lambert_Latin1(buffer)),
    latin2(GDS_Lambert_Latin2(buffer)),
    latitude_south_pole(GDS_Lambert_LatSP(buffer)),
    longitude_south_pole(GDS_Lambert_LonSP(buffer)){}

const char* GridDefinition<RepresentationType::LAMBERT>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>::GridDefinition(unsigned char* buffer){
    throw std::invalid_argument("Unable to use Oblique Lambert Conformal coordinate-system");
}

const char* GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>::print_grid_info() const{
    assert(false);//still not available
}