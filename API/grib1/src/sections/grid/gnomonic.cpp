#include <sections/grid/gnomonic.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>
#include <stdexcept>

GridDefinition<RepresentationType::GNOMONIC>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}
const char* GridDefinition<RepresentationType::GNOMONIC>::print_grid_info() const{
    return "Gnomonic grid. Information unable";
}