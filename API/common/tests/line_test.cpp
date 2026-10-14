#include "common/geometry/Line.h"
#include "common/geometry/Point.h"
#include <gtest/gtest.h>

TEST(Osterlib_line,point_straight_comparision){
    {
        Line line(0,0,1,0);
        ASSERT_TRUE(Line::straight_under_the_point(line,0.5,0.5));
        ASSERT_TRUE(Line::straight_above_the_point(line,0.5,-0.5));
        ASSERT_TRUE(Line::straight_not_under_the_point(line,0.5,-0.5));
        ASSERT_TRUE(Line::straight_not_above_the_point(line,0.5,0.));
        ASSERT_TRUE(Line::straight_not_above_the_point(line,0.5,-0.));
        ASSERT_TRUE(Line::straight_not_under_the_point(line,0.5,0.));
        ASSERT_TRUE(Line::straight_not_under_the_point(line,0.5,-0.));
        ASSERT_TRUE(Line::point_on_straight(line,0.5,-0.));
        ASSERT_TRUE(Line::point_on_straight(line,0.5,0.));
        ASSERT_TRUE(Line::point_on_straight(line,1000,-0.));
        ASSERT_TRUE(Line::point_on_straight(line,-1000,-0.));
        ASSERT_FALSE(Line::point_on_straight(line,-1000,-1.));
        ASSERT_FALSE(Line::point_on_straight(line,1000,-1.));
        ASSERT_FALSE(Line::point_on_straight(line,-1000,1.));
        ASSERT_FALSE(Line::point_on_straight(line,1000,1.));
    }
    {
        Line line(0,0,1,1);
        ASSERT_TRUE(Line::straight_under_the_point(line,0.5,0.6));
        ASSERT_TRUE(Line::straight_above_the_point(line,0.5,0.4));
        ASSERT_TRUE(Line::straight_not_under_the_point(line,0.5,0.4));
        ASSERT_TRUE(Line::straight_not_above_the_point(line,0.5,0.5));
        ASSERT_TRUE(Line::point_on_straight(line,0.5,0.5));
        ASSERT_TRUE(Line::point_on_straight(line,1000,1000.));
        ASSERT_TRUE(Line::point_on_straight(line,-1000,-1000.));
        ASSERT_FALSE(Line::point_on_straight(line,-1000,-1001.));
        ASSERT_FALSE(Line::point_on_straight(line,1000,-1001.));
        ASSERT_FALSE(Line::point_on_straight(line,-1000,1001.));
        ASSERT_FALSE(Line::point_on_straight(line,1000,1001.));
    }
}

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
