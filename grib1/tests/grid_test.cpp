#include "sections/grid/grid.h"
#include <gtest/gtest.h>

TEST(Serialization,Albers){
    grid::GridDefinition<RepresentationType::ALBERS_EQUAL_AREA> grid;
}