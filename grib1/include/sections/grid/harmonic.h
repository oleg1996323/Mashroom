#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"

#ifdef __cplusplus
#include <span>
template<RepresentationType>
struct GridDefinition;

template<>
struct GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer){
        throw std::invalid_argument("Unable to use Harmonic Coefficients coordinate-system");
    }
};

template<>
struct GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer){
        throw std::invalid_argument("Unable to use Rotated Harmonic Coefficients coordinate-system");
    }
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer){
        throw std::invalid_argument("Unable to use Stretched Harmonic Coefficients coordinate-system");
    }
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer){
        throw std::invalid_argument("Unable to use Stretched Rotated Harmonic Coefficients coordinate-system");
    }
};
#endif