#pragma once
#include "code_tables/table_6.h"
#include <cstdint>
#include "code_tables/table_8.h"
#include "code_tables/table_7.h"
#include "def.h"
#include <span>

template<>
struct GridDefinition<RepresentationType::SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};

template<>
struct GridDefinition<RepresentationType::ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};

template<>
struct GridDefinition<RepresentationType::STRETCHED_ROTATED_SPHERICAL_HARMONIC_COEFFICIENTS>{
    GridDefinition(unsigned char* buffer);
    const char* print_grid_info() const;
    void serialize(std::vector<char>& buf) const;
    void deserialize(const std::vector<char>& buf);
};