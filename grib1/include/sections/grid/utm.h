#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "grid_base.h"

namespace grid{
template<>
struct GridDefinition<RepresentationType::UTM>:
    GridDefinitionBase<RepresentationType::MERCATOR,GridModification::NONE>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::UTM>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};
}