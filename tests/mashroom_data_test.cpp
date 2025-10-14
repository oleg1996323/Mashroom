#include <gtest/gtest.h>
#include "program/data.h"

using namespace std::string_view_literals;

class DataTestClass:public Data,public testing::Test{
    protected:
    std::string fn;
    std::vector<ptrdiff_t> pos_;
    std::unordered_set<SearchParamTableVersion> params{ SearchParamTableVersion{.param_=16,.t_ver_=128},
                                                    SearchParamTableVersion{.param_=48,.t_ver_=228}};
    public:
    DataTestClass():fn("data_file.gbd"){
        auto ds = std::make_unique<DataStruct<Data_t::METEO,Data_f::GRIB>>();
        SublimedGribDataInfo sublimed;
        GribProxyDataInfo gribdata;
        grid::GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid;
        grid.base_.dx=1;
        grid.base_.dy=1;
        grid.base_.nx=50;
        grid.base_.ny=50;
        grid.base_.x1=0;
        grid.base_.x2=50;
        grid.base_.y1=50;
        grid.base_.y2=0;
        auto any = path::Storage<false>::file("any_path.grib"sv,utc_tp::clock::now());
        uint64_t count = 0;
        for(int d=0;d<(sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1);++d)
        {
            for(uint8_t table_v = 128;table_v<229;table_v+=228-128)
                for(uint8_t param = 16;param<130;param+=16){
                    ptrdiff_t cur_pos = 1000*count++;
                    gribdata.add_info(any,GribMsgDataInfo(GridInfo(grid),sys_days(year(1990)/month(1)/day(1))+days(d),
                                cur_pos,1000,128,TimeFrame::HOUR,Organization::ECMWF,128));
                    if(params.contains(SearchParamTableVersion{.param_=param,.t_ver_=table_v}));
                        pos_.push_back(cur_pos);
                }
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
    auto matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                        Organization::ECMWF,
                                        TimeFrame::HOUR,
                                        params,
                                        TimeInterval{.from_=sys_days(year(1990)/month(1)/day(1)),
                                            .to_=sys_days(year(1990)/month(1)/day(31))},
                                        RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                                        Coord{.lat_=25,.lon_=25});
    EXPECT_EQ(matched_grib1.size(),( sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1)*params.size());
    {
        EXPECT_EQ(pos_,matched_grib1);
    }
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}