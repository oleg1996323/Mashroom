#include <sections/grid/lambert.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>

using namespace grid;

GridDefinition<RepresentationType::LAMBERT>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::LAMBERT>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}

const char* GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>::print_grid_info() const{
    assert(false);//still not available
}