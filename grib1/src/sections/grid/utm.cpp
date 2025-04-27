#include <sections/grid/utm.h>
#include <stdexcept>
#include <cassert>

GridDefinition<RepresentationType::UTM>::GridDefinition(unsigned char* buffer){
    throw std::invalid_argument("Unable to use UTM coordinate-system");
}

const char* GridDefinition<RepresentationType::UTM>::print_grid_info() const{
    assert(false);
}