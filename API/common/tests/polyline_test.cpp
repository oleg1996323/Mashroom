#include <gtest/gtest.h>
#include "common/geometry/Polyline.h"

TEST(polyline,cut_test){
    Polyline pl;
    pl.append({.lat_=0,.lon_=0});
    pl.append({.lat_=1,.lon_=1});
    pl.append({.lat_=4,.lon_=2});
    pl.append({.lat_=-2,.lon_=4});
    pl.append({.lat_=2,.lon_=6});
    pl.append({.lat_=4,.lon_=9});
    Polyline cut_pl;
    cut_pl.append({.lat_=-1,.lon_=1});
    cut_pl.append({.lat_=6,.lon_=2});
    cut_pl.append({.lat_=4,.lon_=2.5});
    cut_pl.append({.lat_=1,.lon_=2.5});
    cut_pl.append({.lat_=1,.lon_=6});
    cut_pl.append({.lat_=5,.lon_=9});
    cut_pl.append({.lat_=-2,.lon_=2});
    auto polylines = pl.cut(cut_pl,true);
    print(polylines);
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}