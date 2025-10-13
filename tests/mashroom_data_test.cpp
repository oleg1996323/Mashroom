#include <gtest/gtest.h>
#include "program/data.h"

using namespace std::string_view_literals;

class DataTestClass:public Data,public testing::Test{
    protected:
    std::string fn;
    public:
    DataTestClass():fn("data_file.gbd"){
        auto ds = std::make_unique<DataStruct<Data_t::METEO,Data_f::GRIB>>();
        SublimedGribDataInfo sublimed;
        GribDataInfo gribdata;
        grid::GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid;
        grid.base_.dx=1;
        grid.base_.dy=1;
        grid.base_.nx=50;
        grid.base_.ny=50;
        grid.base_.x1=0;
        grid.base_.x2=50;
        grid.base_.y1=0;
        grid.base_.y2=50;
        auto any = path::Storage<false>::file("any_path.grib"sv,utc_tp::clock::now());
        for(int h=0;h<8760;++h)
        {
            for(int table_v = 128;table_v<229;table_v+=228-128)
                for(int param = 16;param<130;param+=16)
                    gribdata.add_info(any,GribMsgDataInfo(GridInfo(grid),sys_days(year(1990)/month(1)/day(1))+hours(h),
                                300000*h,300000,128,TimeFrame::HOUR,Organization::ECMWF,128));
        }
        sublimed.add_data(gribdata);
        ds->add_data(sublimed);
        
        std::ofstream stream(fn,std::ofstream::trunc|std::ofstream::out);
        serialization::serialize_to_file(*ds,stream);
    }

    ~DataTestClass(){
        assert(std::filesystem::remove(fn));
    }
};



TEST_F(DataTestClass,InitTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::METEO,Data_f::GRIB>();
    EXPECT_TRUE(dstruct.sublimed_.paths().contains(path::Storage<false>::file("any_path.grib"sv,utc_tp())));
    EXPECT_TRUE(dstruct.sublimed_.data().contains(path::Storage<false>::file("any_path.grib"sv,utc_tp())));
}

TEST_F(DataTestClass,MatchTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::METEO,Data_f::GRIB>();
    std::unordered_set<SearchParamTableVersion> params{ SearchParamTableVersion{.param_=16,.t_ver_=128},
                                                        SearchParamTableVersion{.param_=48,.t_ver_=228}};
    auto matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                        Organization::ECMWF,
                                        TimeFrame::HOUR,
                                        params,
                                        TimeInterval{.from_=sys_days(year(1990)/month(1)/day(1)),
                                            .to_=sys_days(year(1990)/month(12)/day(25))},
                                        RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                                        Coord{.lat_=25,.lon_=25});
    EXPECT_EQ(matched_grib1.size(),( sys_days(year(1990)/month(12)/day(25))-
                                        sys_days(year(1990)/month(1)/day(1)))/hours(1)*params.size());
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}