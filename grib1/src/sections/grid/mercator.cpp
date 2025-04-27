#include <sections/grid/mercator.h>
#include <cassert>

GridDefinition<RepresentationType::MERCATOR>::GridDefinition(unsigned char* buffer):
    nx(GDS_Merc_nx(buffer)),
    ny(GDS_Merc_ny(buffer)),
    y1(0.001*GDS_Merc_La1(buffer)),
    x1(0.001*GDS_Merc_Lo1(buffer)),
    resolutionAndComponentFlags(ResolutionComponentFlags(GDS_Merc_mode(buffer))),
    y2(0.001*GDS_Merc_La2(buffer)),
    x2(0.001*GDS_Merc_Lo2(buffer)),
    dy(0.001*GDS_Merc_dy(buffer)),
    dx(0.001*GDS_Merc_dx(buffer)),
    scan_mode(ScanMode(GDS_Merc_mode(buffer))),
    latin(GDS_Merc_Latin(buffer)){}

const char* GridDefinition<RepresentationType::MERCATOR>::print_grid_info() const{
    assert(false);
}