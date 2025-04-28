#include <sections/grid/harmonic.h>
#include <sections/grid/grid.h>
#include <format>
#include <cassert>
#include <stdexcept>

GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer){
    assert(false);
    throw std::invalid_argument("Unable to use Harmonic Coefficients coordinate-system");
}

const char* GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    assert(false);//still not available
}

GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer){
    assert(false);
    throw std::invalid_argument("Unable to use Rotated Harmonic Coefficients coordinate-system");
}

const char* GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    assert(false);//still not available
}

GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer){
    assert(false);
    throw std::invalid_argument("Unable to use Stretched Harmonic Coefficients coordinate-system");
}

const char* GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    assert(false);//still not available
}

GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::GridDefinition(unsigned char* buffer){
    assert(false);
    throw std::invalid_argument("Unable to use Stretched Rotated Harmonic Coefficients coordinate-system");
}

const char* GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>::print_grid_info() const{
    assert(false);//still not available
}