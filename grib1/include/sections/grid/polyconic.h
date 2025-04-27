#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"

template<>
struct GridDefinition<RepresentationType::SIMPLE_POLYCONIC>{    
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
};