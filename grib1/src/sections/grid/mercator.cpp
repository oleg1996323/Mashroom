#include <sections/grid/mercator.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

using namespace grid;

GridDefinition<RepresentationType::MERCATOR>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::MERCATOR>::print_grid_info() const{
    assert(false);
}