#include <sections/grid/gaussian.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

GridDefinition<RepresentationType::GAUSSIAN>::GridDefinition(unsigned char* buffer):
    y1(0.001*GDS_LatLon_La1(buffer)),
    x1(0.001*GDS_LatLon_Lo1(buffer)),
    y2(0.001*GDS_LatLon_La2(buffer)),
    x2(0.001*GDS_LatLon_Lo2(buffer)),
    nx(GDS_LatLon_nx(buffer)),
    ny(GDS_LatLon_ny(buffer)),
    directionIncrement(GDS_LatLon_dx(buffer)),
    N(GDS_Gaussian_nlat(buffer)),
    scan_mode(ScanMode(GDS_LatLon_scan(buffer))),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))){}

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
    GridDefinition<RepresentationType::GAUSSIAN>(buffer),
    yp(std::bit_cast<int32_t>(read_bytes(buffer[32],buffer[33],buffer[34]))),
    xp(std::bit_cast<int32_t>(read_bytes(buffer[35],buffer[36],buffer[37]))),
    ang(ibm2flt(&buffer[38])){}

bool GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::operator==(const GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>& other) const{
    return  *static_cast<const GridDefinition<RepresentationType::GAUSSIAN>*>(this)==other &&
            yp==other.yp &&
            xp==other.xp &&
            ang==other.ang;
}
const char* GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinition<RepresentationType::GAUSSIAN>(buffer){}

const char* GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
bool GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::operator==(const GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>& other) const{
    return  *static_cast<const GridDefinition<RepresentationType::GAUSSIAN>*>(this)==other &&
            ysp==other.ysp &&
            xsp==other.xsp &&
            s_factor==other.s_factor;
}
GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>(buffer),GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>(buffer),
    GridDefinition<RepresentationType::GAUSSIAN>(buffer)
    {}

const char* GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
bool GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::operator==(const GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>& other) const{
    return  *static_cast<const GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>*>(this)==other && 
            yp==other.yp &&
            xp==other.xp &&
            ang==other.ang;
}