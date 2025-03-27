#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include "stdexcept"

#ifdef __cplusplus
#pragma pack(push,1)
template<RepresentationType>
struct GridDefinition;

template<>
struct GridDefinition<RepresentationType::GNOMONIC>{
    GridDefinition(char* buffer){
        throw std::invalid_argument("Unable to use Gnomonic coordinate-system");
    }
};
#pragma pack(pop)
#endif