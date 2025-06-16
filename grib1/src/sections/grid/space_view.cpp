#include <sections/grid/space_view.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>

using namespace grid;

GridDefinition<RepresentationType::SPACE_VIEW>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}
const char* GridDefinition<RepresentationType::SPACE_VIEW>::print_grid_info() const{
    assert(false);
}