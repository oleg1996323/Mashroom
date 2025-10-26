#include "sections/grid/grid.h"
#include <gtest/gtest.h>

TEST(Serialization,Albers){
    grid::GridDefinition<RepresentationType::ALBERS_EQUAL_AREA> grid;
}

TEST(GridTest,PosInGridTest){
    grid::GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid;
        grid.base_.dx=1;
        grid.base_.dy=1;
        grid.base_.nx=50;
        grid.base_.ny=50;
        grid.base_.x1=0;
        grid.base_.x2=50;
        grid.base_.y1=50;
        grid.base_.y2=0;
    ASSERT_TRUE(pos_in_grid(Coord{.lat_=25,.lon_=25},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=-10,.lon_=-10},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=-10,.lon_=10},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=10,.lon_=-10},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=10,.lon_=51},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=51,.lon_=10},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=51,.lon_=51},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=-180,.lon_=-180},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=180,.lon_=180},GridInfo(grid)));
    ASSERT_TRUE(pos_in_grid(Coord{.lat_=50,.lon_=50},GridInfo(grid)));
    ASSERT_TRUE(pos_in_grid(Coord{.lat_=0,.lon_=0},GridInfo(grid)));
    ASSERT_TRUE(pos_in_grid(Coord{.lat_=50,.lon_=25},GridInfo(grid)));
    ASSERT_TRUE(pos_in_grid(Coord{.lat_=0,.lon_=25},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=60,.lon_=50},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=-10,.lon_=50},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=-10,.lon_=0},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=60,.lon_=0},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=50,.lon_=-10},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=50,.lon_=60},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=0,.lon_=-10},GridInfo(grid)));
    ASSERT_FALSE(pos_in_grid(Coord{.lat_=0,.lon_=60},GridInfo(grid)));
}


int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}