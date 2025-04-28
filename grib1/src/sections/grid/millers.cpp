#include <sections/grid/millers.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

GridDefinition<RepresentationType::MILLERS_CYLINDR>::GridDefinition(unsigned char* buffer):
resolutionAndComponentFlags(GDS_Lambert_mode(buffer)),scan_mode(GDS_Lambert_scan(buffer)){
    throw std::invalid_argument("Unable to use Millers coordinate-system");
}

const char* GridDefinition<RepresentationType::MILLERS_CYLINDR>::print_grid_info() const{
    assert(false);
}