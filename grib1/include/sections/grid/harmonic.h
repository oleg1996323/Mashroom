#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <span>
#include "code_tables/table_10.h"
#include "network/common/utility.h"
#include "grid_base.h"

namespace grid{
/** @brief Spherical harmonic grid
    @note Notes: 
    1.  The pentagonal representation of resolution is general.
        Some common truncations are special cases of the pentagonal one: Triangular M = J = K Rhomboidal K = J + M Trapezoidal K = J, K > M
    2.  The representation type (octet 13) indicates the method used to define the norm.
    3.  The representation mode (octet 14) indicates the order of the coefficients, whether global or hemispheric data are depicted,
        and the nature of the parameter stored (symmetric or antisymmetric).
    4.  See Notes (6) to (11) under Grid definition latitude/longitude grid (or equidistant cylindrical, or Plate Carrée) page I.2 Bi 8.

    /// @attention bytes 15-32: reserved
*/
template<>
struct GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>:
    GridDefinitionBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS,GridModification::NONE>{   
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};

/** @brief Rotated pherical harmonic grid
    /// @attention bytes 15-32: reserved
*/
template<>
struct GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>:
    GridDefinitionBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS,GridModification::ROTATION>{
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};

/** @brief Stretched pherical harmonic grid
    /// @attention bytes 15-32: reserved
*/
template<>
struct GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>:
        GridDefinitionBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS,GridModification::STRETCHING>{
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>:
    GridDefinitionBase<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS,GridModification::ROTATION_STRETCHING>{
    GridDefinition(unsigned char* buffer);
    /// @todo
    /// @return Printed by text parameters
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};

}