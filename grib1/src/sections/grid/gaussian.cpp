#include <sections/grid/gaussian.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>

GridDefinition<RepresentationType::GAUSSIAN>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}
/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::GAUSSIAN>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::ROTATED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::STRETCHED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}
GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::GridDefinition(unsigned char* buffer):
    GridDefinitionBase(buffer){}

/// @todo
/// @return Printed by text parameters
const char* GridDefinition<RepresentationType::STRETCHED_ROTATED_GAUSSIAN_LAT_LON>::print_grid_info() const{
    assert(false);//still not available
}