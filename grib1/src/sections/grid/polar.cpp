#include <sections/grid/polar.h>
#include <cassert>

GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>::GridDefinition(unsigned char* buffer):
    nx(GDS_Polar_nx(buffer)),
    ny(GDS_Polar_ny(buffer)),
    y1(0.001*GDS_Polar_La1(buffer)),
    x1(0.001*GDS_Polar_Lo1(buffer)),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Polar_mode(buffer))),
    LoV(0.001*GDS_Polar_Lov(buffer)),
    Dy(GDS_Polar_Dy(buffer)),
    Dx(GDS_Polar_Dx(buffer)),
    is_south_pole(GDS_Polar_pole(buffer)),
    scan_mode(ScanMode(GDS_Polar_mode(buffer))){}

const char* GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>::print_grid_info() const{
    assert(false);
}