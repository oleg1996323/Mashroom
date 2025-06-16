#pragma once
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "grid_base.h"

namespace grid{
template<>
struct GridDefinition<RepresentationType::POLAR_STEREOGRAPH_PROJ>:
    GridDefinitionBase<RepresentationType::POLAR_STEREOGRAPH_PROJ,GridModification::NONE>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
};
}