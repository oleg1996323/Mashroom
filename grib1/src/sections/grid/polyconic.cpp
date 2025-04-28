#include <sections/grid/polyconic.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>
  
GridDefinition<RepresentationType::SIMPLE_POLYCONIC>::GridDefinition(unsigned char* buffer){
    throw std::invalid_argument("Unable to use Polyconic coordinate-system");
}
const char* GridDefinition<RepresentationType::SIMPLE_POLYCONIC>::print_grid_info() const{
    assert(false);
}