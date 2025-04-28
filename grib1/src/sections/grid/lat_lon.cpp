#include <sections/grid/lat_lon.h>
#include <sections/grid/grid.h>
#include <format>
GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>::GridDefinition(unsigned char* buffer):
    nx(GDS_LatLon_nx(buffer)),
    ny(GDS_LatLon_ny(buffer)),
    y1(0.001*GDS_LatLon_La1(buffer)),
    x1(0.001*GDS_LatLon_Lo1(buffer)),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_LatLon_mode(buffer))),
    y2(0.001*GDS_LatLon_La2(buffer)),
    x2(0.001*GDS_LatLon_Lo2(buffer)),
    dy(0.001*GDS_LatLon_dy(buffer)),
    dx(0.001*GDS_LatLon_dx(buffer)),
    scan_mode(ScanMode(GDS_LatLon_scan(buffer))){}

bool GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>::operator==(const GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>& other) const{
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

const char* GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR>::print_grid_info() const{
    return std::format("grid {}({}) lat{}-lon{}:lat{}-lon{} dx {} dy {}",
        (int)RepresentationType::ROTATED_LAT_LON,grid_to_abbr(RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR),
        x1,y1,x2,y2,dx,dy).c_str();
}

GridDefinition<RepresentationType::ROTATED_LAT_LON>::GridDefinition(unsigned char* buffer):
    nx(GDS_RotLL_nx(buffer)),
    ny(GDS_RotLL_ny(buffer)),
    y1(0.001*GDS_RotLL_La1(buffer)),
    x1(0.001*GDS_RotLL_Lo1(buffer)),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_RotLL_mode(buffer))),
    y2(0.001*GDS_RotLL_La2(buffer)),
    x2(0.001*GDS_RotLL_Lo2(buffer)),
    dy(0.001*GDS_RotLL_dy(buffer)),
    dx(0.001*GDS_RotLL_dx(buffer)),
    scan_mode(ScanMode(GDS_RotLL_scan(buffer))), //TODO: correct (see above)
    angle_rotation(GDS_RotLL_RotAng(buffer)),
    latitude_south_pole(GDS_RotLL_LaSP(buffer)),
    longitude_south_pole(GDS_RotLL_LoSP(buffer)){}
bool GridDefinition<RepresentationType::ROTATED_LAT_LON>::operator==(const GridDefinition<RepresentationType::ROTATED_LAT_LON>& other) const{
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

const char* GridDefinition<RepresentationType::ROTATED_LAT_LON>::print_grid_info() const{
    return std::format("grid {}({}) lat{}-lon{}:lat{}-lon{} dx {} dy {}",
        (int)RepresentationType::ROTATED_LAT_LON,grid_to_abbr(RepresentationType::ROTATED_LAT_LON),
        x1,y1,x2,y2,dx,dy).c_str();
}

GridDefinition<RepresentationType::STRETCHED_LAT_LON>::GridDefinition(unsigned char* buffer):
    nx(GDS_StrLL_nx(buffer)),
    ny(GDS_StrLL_ny(buffer)),
    y1(0.001*GDS_StrLL_La1(buffer)),
    x1(0.001*GDS_StrLL_Lo1(buffer)),
    resolutionAndComponentFlags((ResolutionComponentFlags)GDS_StrLL_mode(buffer)),
    y2(0.001*GDS_StrLL_La2(buffer)),
    x2(0.001*GDS_StrLL_Lo2(buffer)),
    dy(0.001*GDS_StrLL_dy(buffer)),
    dx(0.001*GDS_StrLL_dx(buffer)),
    scan_mode(ScanMode(GDS_StrLL_scan(buffer))),
    stretch_factor(GDS_StrLL_StrFactor(buffer)),
    latitude_stretch_pole(GDS_StrLL_LaStrP(buffer)),
    longitude_stretch_pole(GDS_StrLL_LoStrP(buffer)){}

bool GridDefinition<RepresentationType::STRETCHED_LAT_LON>::operator==(const GridDefinition<RepresentationType::STRETCHED_LAT_LON>& other) const{
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

const char* GridDefinition<RepresentationType::STRETCHED_LAT_LON>::print_grid_info() const{
    return std::format("grid {}({}) lat{}-lon{}:lat{}-lon{} dx {} dy {}",
        (int)RepresentationType::ROTATED_LAT_LON,grid_to_abbr(RepresentationType::STRETCHED_LAT_LON),
        x1,y1,x2,y2,dx,dy).c_str();
}

GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>::GridDefinition(unsigned char* buffer):
    nx(GDS_StrLL_nx(buffer)),
    ny(GDS_StrLL_ny(buffer)),
    y1(0.001*GDS_StrLL_La1(buffer)),
    x1(0.001*GDS_StrLL_Lo1(buffer)),
    resolutionAndComponentFlags((ResolutionComponentFlags)GDS_StrLL_mode(buffer)),
    y2(0.001*GDS_StrLL_La2(buffer)),
    x2(0.001*GDS_StrLL_Lo2(buffer)),
    dy(0.001*GDS_StrLL_dy(buffer)),
    dx(0.001*GDS_StrLL_dx(buffer)),
    scan_mode(ScanMode(GDS_StrLL_mode(buffer))),
    stretch_factor(GDS_StrLL_StrFactor(buffer)),
    latitude_south_pole(GDS_RotLL_LaSP(buffer)),
    longitude_south_pole(GDS_RotLL_LoSP(buffer)),
    latitude_stretch_pole(GDS_StrLL_LaStrP((&buffer[43-33]))),
    longitude_stretch_pole(GDS_StrLL_LoStrP((&buffer[43-33]))){}

bool GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>::operator==(const GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>& other) const{
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

const char* GridDefinition<RepresentationType::STRETCHED_AND_ROTATED_LAT_LON>::print_grid_info() const{
    return std::format("grid {}({}) lat{}-lon{}:lat{}-lon{} dx {} dy {}",
        (int)RepresentationType::ROTATED_LAT_LON,grid_to_abbr(RepresentationType::STRETCHED_AND_ROTATED_LAT_LON),
        x1,y1,x2,y2,dx,dy).c_str();
}