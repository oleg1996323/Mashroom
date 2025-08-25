#include "sections/grid/grid.h"
#include <gtest/gtest.h>

TEST(Serialization,Albers){
    grid::GridDefinition<RepresentationType::ALBERS_EQUAL_AREA> grid;
}


int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}