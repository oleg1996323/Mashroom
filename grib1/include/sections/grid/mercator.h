#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "float.h"
#include <span>
#include "grid_base.h"

namespace grid{

template<>
struct GridDefinition<RepresentationType::MERCATOR>:
    GridDefinitionBase<RepresentationType::MERCATOR,GridModification::NONE>{

    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    bool operator==(const GridDefinition<RepresentationType::MERCATOR>& other) const{
        return GridDefinitionBase::operator==(other);
    }
};
}