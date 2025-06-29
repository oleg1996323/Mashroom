#include <sections/grid/utm.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>

GridDefinition<RepresentationType::UTM>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::UTM>::print_grid_info() const{
    assert(false);
}