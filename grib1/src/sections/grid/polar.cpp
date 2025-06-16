#include <sections/grid/polar.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

using namespace grid;

GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>::print_grid_info() const{
    assert(false);
}