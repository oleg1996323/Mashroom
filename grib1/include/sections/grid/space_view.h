#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "grid_base.h"

namespace grid{
template<>
struct GridDefinition<RepresentationType::SPACE_VIEW>:
    GridDefinitionBase<RepresentationType::SPACE_VIEW,GridModification::NONE>{
    GridDefinition(unsigned char* buffer);
    bool operator==(const GridDefinition<RepresentationType::SPACE_VIEW>& other) const{
        return GridDefinitionBase::operator==(other);
    }
    const char* print_grid_info() const;
};
}