#include <sections/grid/space_view.h>
#include <sections/grid/grid.h>
#include <format>
#include <stdexcept>
#include <cassert>
GridDefinition<RepresentationType::SPACE_VIEW>::GridDefinition(unsigned char* buffer){
    throw std::invalid_argument("Unable to use Space-view coordinate-system");
}
const char* GridDefinition<RepresentationType::SPACE_VIEW>::print_grid_info() const{
    assert(false);
}