#include <sections/grid/albers.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>
#include "network/common/utility.h"

using namespace network::utility;

GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::ALBERS_EQUAL_AREA>::print_grid_info() const{
    assert(false);//still not available
}