#include <sections/grid/millers.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

GridDefinition<RepresentationType::MILLERS_CYLINDR>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::MILLERS_CYLINDR>::print_grid_info() const{
    assert(false);
}