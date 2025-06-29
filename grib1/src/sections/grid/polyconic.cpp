#include <sections/grid/polyconic.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>
  
using namespace grid;

GridDefinition<RepresentationType::SIMPLE_POLYCONIC>::GridDefinition(unsigned char* buffer):
GridDefinitionBase(buffer){}
const char* GridDefinition<RepresentationType::SIMPLE_POLYCONIC>::print_grid_info() const{
    assert(false);
}