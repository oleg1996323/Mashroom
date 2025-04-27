#include "sections/grid/gnomonic.h"
#include <cassert>
#include <stdexcept>

GridDefinition<RepresentationType::GNOMONIC>::GridDefinition(unsigned char* buffer){
    throw std::invalid_argument("Unable to use Gnomonic coordinate-system");
}
const char* GridDefinition<RepresentationType::GNOMONIC>::print_grid_info() const{
    assert(false);
}