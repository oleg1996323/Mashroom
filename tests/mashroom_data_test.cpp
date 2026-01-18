#include <gtest/gtest.h>
#include "program/data.h"

using namespace std::string_view_literals;

TEST(CommonDataProperties,HashTest){
    Grib1CommonDataProperties cmn;
    ASSERT_EQ(cmn.hash(),(static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<Organization>>::max())<<56|
                static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<TimeFrame>>::max())<<48|
                static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<TimeRangeIndicator>>::max())<<40|
                static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<32|
                static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<24|
                static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<16|
                static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<8|
                std::numeric_limits<uint8_t>::max()));
    
}

TEST(VariationParameters,get_parameter_variations_test){
    auto vars = Grib1Data::get_parameter_variations(Organization::ECMWF,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),{{128,128},{228,128}});
    ASSERT_EQ(vars.size(),2);
    vars = Grib1Data::get_parameter_variations(Organization::ECMWF,std::nullopt,Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),{{128,128},{228,128}});
    ASSERT_EQ(vars.size(),2*std::count(is_time.begin(),is_time.end(),1));
}

class DataTestClass:public Data,public testing::Test{
    protected:
    std::string fn;
    std::vector<ptrdiff_t> pos_;
    std::unordered_set<SearchParamTableVersion> params{ SearchParamTableVersion{.param_=16,.t_ver_=128},
                                                    SearchParamTableVersion{.param_=48,.t_ver_=228}};
    public:
    DataTestClass():fn("data_file.g1bd"){
        auto ds = std::make_unique<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>();
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
        for(int d=0;d<=(sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1);++d)
        {
            for(int table_v = 128;table_v<229;table_v+=228-128)
                for(int param = 16;param<130;param+=16){
                    ptrdiff_t cur_pos = 1000*count++;
                    gribdata.add_info(any,GribMsgDataInfo(GridInfo(grid),sys_days(year(1990)/month(1)/day(1))+days(d),
                                cur_pos,1000,param,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                Organization::ECMWF,table_v,
                                Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0)));
                    if(params.contains(SearchParamTableVersion{.param_=static_cast<uint8_t>(param),.t_ver_=static_cast<uint8_t>(table_v)}))
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

class DataTestClass_1:public Data,public testing::Test{
    protected:
    std::string fn;
    size_t cmn_sz = 0;
    std::vector<Organization> centers = {Organization::WMO,Organization::ECMWF,Organization::DWD};
    std::vector<std::pair<Organization,std::vector<uint8_t>>> tables_by_id_ = {{Organization::ECMWF,{228,180,210,130,211,162}},
                                                                            {Organization::WMO,{1,2,3}}};
    public:
    DataTestClass_1():fn("data_file.g1bd"){
        auto ds = std::make_unique<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>>();
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
        path::Storage<false> any;
        for(int id = 1;id<=tables_by_id_.size();++id){
            uint64_t count = 0;
            any = path::Storage<false>::file("any_path_"s+std::to_string(id)+".grib"s,utc_tp::clock::now());
            for(int d=0;d<=(sys_days(year(1990)/month(id+1)/day(1))-days(1)-sys_days(year(1990)/month(id)/day(1)))/days(1);++d)
            {
                for(auto table:tables_by_id_[id-1].second)
                    for(int param = 16+id;param<130+id;param+=16+id){
                        ptrdiff_t cur_pos = 1000*count++;
                        gribdata.add_info(any,GribMsgDataInfo(GridInfo(grid),sys_days(year(1990)/month(id)/day(1))+days(d),
                                    cur_pos,1000+2000*id,param,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),tables_by_id_[id-1].first,table,Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0)));
                    }
            }
        }
        for(auto& [fn,data]:gribdata.data())
            cmn_sz+=data.size();
        sublimed.add_data(gribdata);
        ds->add_data(sublimed);
        std::ofstream stream(fn,std::ofstream::trunc|std::ofstream::out);
        serialization::serialize_to_file(*ds,stream);
    }

    ~DataTestClass_1(){
        assert(std::filesystem::remove(fn));
    }
};

TEST_F(DataTestClass,InitTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    ASSERT_TRUE(dstruct.sublimed_.paths().contains(path::Storage<false>::file("any_path.grib"sv,utc_tp())));
    ASSERT_TRUE(dstruct.sublimed_.data().contains(path::Storage<false>::file("any_path.grib"sv,utc_tp())));
    int count = 0;
}

TEST_F(DataTestClass,MatchTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    auto matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                        Organization::ECMWF,
                                        TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                        Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                        params,
                                        TimeInterval(sys_days(year(1990)/month(1)/day(1)),
                                            sys_days(year(1990)/month(1)/day(31))),
                                        RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                                        Coord{.lat_=25,.lon_=25});
    EXPECT_EQ(matched_grib1.size(),((sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1)+1)*params.size());
    EXPECT_EQ(pos_,matched_grib1);
    EXPECT_EQ(pos_.size(),matched_grib1.size());
    matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                        Organization::ECMWF,
                                        TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                        Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                        params,
                                        TimeInterval(sys_days(year(1990)/month(1)/day(1)),
                                            sys_days(year(1990)/month(2)/day(1))),
                                        RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                                        Coord{.lat_=25,.lon_=25});
    EXPECT_TRUE(matched_grib1.empty());
    matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                    Organization::ECMWF,
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    params,
                                    TimeInterval(sys_days(year(1990)/month(1)/day(1)),
                                        sys_days(year(1990)/month(2)/day(1))),
                                    RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                                    Coord{.lat_=-10,.lon_=25});
    EXPECT_TRUE(matched_grib1.empty());
    matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                    Organization::ECMWF,
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    params,
                                    TimeInterval(sys_days(year(1989)/month(1)/day(1)),
                                        sys_days(year(1989)/month(12)/day(31))),
                                    RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                                    Coord{.lat_=25,.lon_=25});
    EXPECT_TRUE(matched_grib1.empty());
    matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                    Organization::ECMWF,
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    params,
                                    TimeInterval(sys_days(year(1989)/month(1)/day(1)),
                                        sys_days(year(1990)/month(1)/day(2))),
                                    RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                                    Coord{.lat_=25,.lon_=25});
    EXPECT_EQ(matched_grib1.size(),((sys_days(year(1990)/month(1)/day(2))-
                                    sys_days(year(1990)/month(1)/day(1)))
                                    /days(1)+1)*params.size());
    matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                    Organization::ECMWF,
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    params,
                                    TimeInterval(sys_days(year(1989)/month(1)/day(1)),
                                        sys_days(year(1990)/month(1)/day(2))),
                                    RepresentationType::GAUSSIAN,
                                    Coord{.lat_=25,.lon_=25});
    EXPECT_TRUE(matched_grib1.empty());
    matched_grib1 = match(path::Storage<false>::file("any_path.grib"sv,utc_tp()),
                                    Organization::WMO,
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    params,
                                    TimeInterval(sys_days(year(1989)/month(1)/day(1)),
                                        sys_days(year(1990)/month(1)/day(2))),
                                    RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                                    Coord{.lat_=25,.lon_=25});
    EXPECT_TRUE(matched_grib1.empty());
}

TEST_F(DataTestClass_1,MatchDataTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    auto matched_data = match_data(Organization::WMO,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                        Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                        {SearchParamTableVersion{.param_=18,.t_ver_ = 1},
                        SearchParamTableVersion{.param_=uint8_t(54),.t_ver_=3}},
                        TimeInterval(sys_days(1990y/2/1),sys_days(1990y/2/2)),
                        RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,Coord{.lat_=25,.lon_=25});
    ASSERT_TRUE(matched_data.contains(path::Storage<false>::file("any_path_2.grib"s,utc_tp())));
    EXPECT_EQ(matched_data.size(),1);
    EXPECT_EQ(matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).buf_pos_.size(),((sys_days(1990y/2/2)-sys_days(1990y/2/1))/days(1)+1)*2);
    ASSERT_TRUE(matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).grid_data_.has_value());
    EXPECT_EQ(matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).grid_data_->index(),1);
    auto sequence = matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).sequence_time_;
    // std::cout<<"Stored: from "<<sequence.get_interval().from()<<" to "<<sequence.get_interval().to()<<std::endl;
    std::error_code err;
    TimeSequence ts(sys_days(1990y/2/1),sys_days(1990y/2/2),err,days(1));
    ASSERT_EQ(err,std::error_code());
    EXPECT_EQ(matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).sequence_time_,ts);
}

TEST_F(DataTestClass_1,FindAllTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    std::error_code err;
    TimeSequence ts(utc_tp(),system_clock::now(),err,days(1));
    ASSERT_EQ(err,std::error_code());
    auto matched_data = find_all(RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                        ts,
                        TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                        Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                        utc_tp());
    EXPECT_EQ(matched_data.size(),cmn_sz);
    ts = TimeSequence(utc_tp(),system_clock::now(),err,days(1));
    ASSERT_EQ(err,std::error_code());
    matched_data = find_all(RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
                        ts,
                        TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                        Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                        utc_tp());
    EXPECT_EQ(matched_data.size(),cmn_sz);
    // EXPECT_EQ(matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).buf_pos_.size(),((sys_days(1990y/2/2)-sys_days(1990y/2/1))/days(1)+1)*2);
    // ASSERT_TRUE(matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).grid_data_.has_value());
    // EXPECT_EQ(matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).grid_data_->index(),1);
    // auto sequence = matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).sequence_time_;
    // std::cout<<"Stored: from "<<sequence.get_interval().from()<<" to "<<sequence.get_interval().to()<<std::endl;
    // EXPECT_EQ(matched_data.at(path::Storage<false>::file("any_path_2.grib"s,utc_tp())).sequence_time_,TimeSequence(sys_days(1990y/2/1),sys_days(1990y/2/2),days(1)));
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}