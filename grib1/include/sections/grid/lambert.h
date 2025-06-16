#pragma once
#include "grid_base.h"

namespace grid{
    template<>
    struct GridDefinition<RepresentationType::LAMBERT>:
        GridDefinitionBase<LAMBERT,GridModification::NONE>{
        GridDefinition(unsigned char* buffer);
        const char* print_grid_info() const;
        bool operator==(const GridDefinition<RepresentationType::LAMBERT>& other) const{
            return GridDefinitionBase::operator==(other);
        }
    };

    template<>
    struct GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>:
        GridDefinitionBase<LAMBERT,GridModification::NONE>{
        GridDefinition(unsigned char* buffer);
        const char* print_grid_info() const;
        bool operator==(const GridDefinition<RepresentationType::OBLIQUE_LAMBERT_CONFORMAL>& other) const{
            return GridDefinitionBase::operator==(other);
        }
    };
}