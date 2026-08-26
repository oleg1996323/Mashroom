#include "common/Polygon.h"
#include <gtest/gtest.h>

TEST(polygon,cut_test){
    Polygon pg;
    pg.append({.lat_=0,.lon_=0});
    pg.append({.lat_=2,.lon_=0});
    pg.append({.lat_=2,.lon_=2});
    pg.append({.lat_=0,.lon_=2});
    auto pgs = pg.cut(Line(1,2,1,0),false);
    ASSERT_EQ(pgs.size(),1);
    Polygon test;
    pg.append({.lat_=0,.lon_=0});
    pg.append({.lat_=2,.lon_=0});
    pg.append({.lat_=2,.lon_=1});
    pg.append({.lat_=0,.lon_=1});
    // EXPECT_EQ(pgs.front(),pg);
}

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
