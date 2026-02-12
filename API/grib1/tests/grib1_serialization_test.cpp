#include "sections/grid/grid.h"
#include "sections/product/time_forecast.h"
#include "sections/product/levels.h"
#include <gtest/gtest.h>

TEST(Grib1StructuresSerialization,GridSerialization_test){
    GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid;
    grid.base_.dx=5;
    grid.base_.dy=10;
    grid.base_.nx=40;
    grid.base_.ny=50;
    ResolutionComponentFlags resol;
    resol.earth_spheroidal=true;
    resol.given_direction=false;
    resol.grid_direction_uv_comp=true;
    grid.base_.resolutionAndComponentFlags = resol;
    ScanMode scan;
    scan.adj_points_j_dir=false;
    scan.points_sub_i_dir=true;
    scan.points_sub_j_dir=true;
    grid.base_.scan_mode=scan;
    grid.base_.x1=90;
    grid.base_.x2=-90;
    grid.base_.y1=40;
    grid.base_.y2=-45;

    GridInfo gi = grid;
    GridInfo other;
    std::vector<char> buf;
    ASSERT_TRUE(serialization::serialize<false>(gi,buf)==serialization::SerializationEC::NONE);
    ASSERT_TRUE(serialization::deserialize<false>(other,std::span<const char>(buf))==serialization::SerializationEC::NONE);
    ASSERT_EQ(gi,other);
}

TEST(Grib1StructuresSerialization,TimeForecastSerialization_test){
    TimeForecast tf(TimeFrame::DAY,
        TimeRangeIndicator::ACCUM_N_UNINIT_INTERVAL_P2,
        TimeForecast::period_t{.val=5},
        TimeForecast::period_t{.val=10}
    );
    TimeForecast other;
    std::vector<char> buf;
    ASSERT_TRUE(serialization::serialize<false>(tf,buf)==serialization::SerializationEC::NONE);
    ASSERT_TRUE(serialization::deserialize<false>(other,std::span<const char>(buf))==serialization::SerializationEC::NONE);
    ASSERT_EQ(tf,other);
}

TEST(Grib1StructuresSerialization,Level_test){
    Level lvl(LevelsTags::GROUND_OR_WATER_SURFACE,10,0);
    Level other;
    std::vector<char> buf;
    ASSERT_TRUE(serialization::serialize<false>(lvl,buf)==serialization::SerializationEC::NONE);
    ASSERT_TRUE(serialization::deserialize<false>(other,std::span<const char>(buf))==serialization::SerializationEC::NONE);
    ASSERT_EQ(lvl,other);
}

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}