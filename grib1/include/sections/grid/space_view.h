#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
template<>
struct GridDefinition<RepresentationType::SPACE_VIEW>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};