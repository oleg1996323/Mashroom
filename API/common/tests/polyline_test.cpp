#include <gtest/gtest.h>
#include "common/geometry/Polyline.h"
#include "boost/geometry.hpp"
#include "boost/geometry/geometries/point.hpp"

using namespace geometry;

TEST(polyline,cut_test){
    Polyline<2>(std::vector<Coord>());
    Polyline<2> pl;
    pl.append(Coord{.lat_=0,.lon_=0});
    pl.append({1,1});
    pl.append(Coord{.lat_=4,.lon_=2});
    pl.append(Coord{.lat_=-2,.lon_=4});
    pl.append(Coord{.lat_=2,.lon_=6});
    pl.append(Coord{.lat_=4,.lon_=9});
    Polyline<2> cut_pl;
    cut_pl.append(Coord{.lat_=-1,.lon_=1});
    cut_pl.append(Coord{.lat_=6,.lon_=2});
    cut_pl.append(Coord{.lat_=4,.lon_=2.5});
    cut_pl.append(Coord{.lat_=1,.lon_=2.5});
    cut_pl.append(Coord{.lat_=1,.lon_=6});
    cut_pl.append(Coord{.lat_=5,.lon_=9});
    cut_pl.append(Coord{.lat_=-2,.lon_=2});
    print(pl);
    BasicLine<2> line(Point<2>({0.5,-0.5}),Point<2>({2,6}));
    auto polylines = pl.cut(line,false);
    print(polylines);
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}