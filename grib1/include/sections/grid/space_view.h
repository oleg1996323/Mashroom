#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"

#ifdef __cplusplus
#include <span>
#include <stdexcept>
template<RepresentationType>
struct GridDefinition;

template<>
struct GridDefinition<RepresentationType::SPACE_VIEW>{
    GridDefinition(unsigned char* buffer){
        throw std::invalid_argument("Unable to use Space-view coordinate-system");
    }
};
#endif