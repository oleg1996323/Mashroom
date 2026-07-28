#include <gtest/gtest.h>
#include "program/data.h"

using namespace std::string_view_literals;

TEST(CommonDataProperties,HashTest){
    Grib1CommonDataProperties cmn;
    ASSERT_EQ(cmn.hash(),(static_cast<uint64_t>(std::numeric_limits<std::underlying_type_t<Organization>>::max())<<16|
                static_cast<uint64_t>(std::numeric_limits<uint8_t>::max())<<8|
                std::numeric_limits<uint8_t>::max()));
}

class GribDataStruct:public testing::Test{
    protected:
    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1> ds;
    std::unordered_set<SearchParamTableVersion> params{ SearchParamTableVersion{.param_=16,.t_ver_=128},
                                                    SearchParamTableVersion{.param_=48,.t_ver_=228}};
    std::vector<ptrdiff_t> pos_;
    public:
    GribDataStruct(){
        grid::GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid;
        grid.base_.dx=1;
        grid.base_.dy=1;
        grid.base_.nx=50;
        grid.base_.ny=50;
        grid.base_.x1=0;
        grid.base_.x2=50;
        grid.base_.y1=50;
        grid.base_.y2=0;
        auto any = Location<false>::file("any_path.grib"s,utc_tp::clock::now());
        uint64_t count = 0;
        std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> msg_data;
        auto err = std::error_code();
        auto param=16;
        auto table_v=228;
        for(int d=0;d<=(sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1);++d)
        {
            // for(int table_v = 128;table_v<229;table_v+=228-128)
            //     for(int param = 16;param<130;param+=16){
                    ptrdiff_t cur_pos = 1000*count++;
                    auto f_error = API::ErrorData::ErrorCode<API_T::GRIB1>::NONE_ERR;
                    auto err = std::error_code();
                    auto msg = data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>(GridInfo(grid),sys_days(year(1990)/month(1)/day(1))+days(d),
                                cur_pos,1000,param,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                Organization::ECMWF,table_v,
                                Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),f_error);
                    msg_data.push_back(std::move(msg));
                    if(params.contains(SearchParamTableVersion{.param_=static_cast<uint8_t>(param),.t_ver_=static_cast<uint8_t>(table_v)}))
                        pos_.push_back(cur_pos);
                //}
        }
        ds.add_data(any,msg_data,err);
    }
};

TEST_F(GribDataStruct,AddDataToGrib1DataStructTest){
    ASSERT_EQ(ds.paths_.size(),1);
    for(auto id:ds.index_){
        for(auto& [ts,pos]:id->ts_pos_)
            ASSERT_EQ(ts.number_of_intervals(),30);
    }
    ASSERT_EQ(ds.by_intervals_.size(),1);
    ASSERT_TRUE(ds.paths_.contains(Location<false>::file("any_path.grib"s)));
}

class DataTestClass:public Data,public testing::Test{
    protected:
    std::string fn;
    std::vector<MessagePositionSizeInfo> pos_;
    std::unordered_set<SearchParamTableVersion> params{ SearchParamTableVersion{.param_=16,.t_ver_=128},
                                                    SearchParamTableVersion{.param_=48,.t_ver_=228}};
    public:
    DataTestClass():fn("data_file.g1bd"){
        DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1> gribdata;
        grid::GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid;
        grid.base_.dx=1;
        grid.base_.dy=1;
        grid.base_.nx=50;
        grid.base_.ny=50;
        grid.base_.x1=0;
        grid.base_.x2=50;
        grid.base_.y1=50;
        grid.base_.y2=0;
        auto any = Location<false>::file("any_path.grib"s,utc_tp::clock::now());
        uint64_t count = 0;
        std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> msg_data;
        auto err = std::error_code();
        for(int d=0;d<=(sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1);++d)
        {
            for(int table_v = 128;table_v<229;table_v+=228-128)
                for(int param = 16;param<130;param+=16){
                    size_t cur_pos = 1000*count++;
                    auto f_error = API::ErrorData::ErrorCode<API_T::GRIB1>::NONE_ERR;
                    
                    auto msg = data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>(GridInfo(grid),sys_days(year(1990)/month(1)/day(1))+days(d),
                                cur_pos,1000,param,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                Organization::ECMWF,table_v,
                                Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),f_error);
                    msg_data.push_back(std::move(msg));
                    if(params.contains(SearchParamTableVersion{.param_=static_cast<uint8_t>(param),.t_ver_=static_cast<uint8_t>(table_v)}))
                        pos_.push_back(MessagePositionSizeInfo{.begin_=cur_pos,.size_=1000});
                }
        }
        gribdata.add_data(any,msg_data,err);
        for(auto id:data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().index_){
            for(auto& [ts,pos]:id->ts_pos_)
                assert(ts.number_of_intervals()==31);
        }
        for(auto id:data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().index_){
            for(auto& [ts,pos]:id->ts_pos_)
                assert(ts.number_of_intervals()==0);
        }
        update_indexing(std::move(gribdata));
        for(auto id:data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().index_){
            for(auto& [ts,pos]:id->ts_pos_)
                assert(ts.number_of_intervals()==30);
        }
        std::ofstream stream(fn,std::ofstream::trunc|std::ofstream::out);
        serialization::serialize_to_file(data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>(),stream);
    }

    void TearDown() override{
        assert(std::filesystem::remove(fn));
    }
};

TEST_F(DataTestClass,InitTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    ASSERT_EQ(dstruct.paths_.size(),1);
    for(auto id:dstruct.index_){
        for(auto& [ts,pos]:id->ts_pos_)
            ASSERT_EQ(ts.number_of_intervals(),30);
    }
    ASSERT_EQ(dstruct.by_intervals_.size(),1);
    ASSERT_TRUE(dstruct.paths_.contains(Location<false>::file("any_path.grib"s)));
    int count = 0;
}

TEST_F(DataTestClass,MatchTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    std::error_code err;
    ASSERT_NE(dstruct.by_intervals_.size(),0);
    auto matched_grib1 = dstruct.match("any_path.grib"sv,
                                        utc_tp(),
                                        Coord{.lat_=25,.lon_=25},
                                        Organization::ECMWF,
                                        params,
                                        sys_days(year(1990)/month(1)/day(1)),
                                        sys_days(year(1990)/month(1)/day(31)),
                                        DateTimeDiff(err,std::chrono::days(1)),
                                        TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                        Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                        RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR);
    EXPECT_EQ(matched_grib1.size(),((sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1)+1)*params.size());
    EXPECT_EQ(pos_,matched_grib1);
    EXPECT_EQ(pos_.size(),matched_grib1.size());
    matched_grib1 = dstruct.match("any_path.grib"sv,
                                        utc_tp(),
                                        Coord{.lat_=25,.lon_=25},
                                        Organization::ECMWF,
                                        params,
                                        sys_days(year(1990)/month(1)/day(1)),
                                        sys_days(year(1990)/month(2)/day(1)),
                                        DateTimeDiff(err,std::chrono::hours(1)),
                                        TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                        Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                        RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR);
    EXPECT_TRUE(matched_grib1.empty());
    matched_grib1 = dstruct.match("any_path.grib"sv,
                                    utc_tp(),
                                    Coord{.lat_=-10,.lon_=25},
                                    Organization::ECMWF,
                                    params,
                                    sys_days(year(1990)/month(1)/day(1)),
                                    sys_days(year(1990)/month(2)/day(1)),
                                    DateTimeDiff(err,std::chrono::hours(1)),
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR);
    EXPECT_TRUE(matched_grib1.empty());
    matched_grib1 = dstruct.match("any_path.grib"sv,
                                    utc_tp(),
                                    Coord{.lat_=25,.lon_=25},
                                    Organization::ECMWF,
                                    params,
                                    sys_days(year(1989)/month(1)/day(1)),
                                    sys_days(year(1989)/month(12)/day(31)),
                                    DateTimeDiff(err,std::chrono::hours(1)),
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR);
    EXPECT_TRUE(matched_grib1.empty());
    matched_grib1 = dstruct.match("any_path.grib"sv,
                                    utc_tp(),
                                    Coord{.lat_=25,.lon_=25},
                                    Organization::ECMWF,
                                    params,
                                    sys_days(year(1989)/month(1)/day(1)),
                                    sys_days(year(1990)/month(1)/day(2)),
                                    DateTimeDiff(err,std::chrono::days(1)),
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR);
    EXPECT_EQ(matched_grib1.size(),((sys_days(year(1990)/month(1)/day(2))-
                                    sys_days(year(1990)/month(1)/day(1)))
                                    /days(1)+1)*params.size());
    matched_grib1 = dstruct.match("any_path.grib"sv,
                                    utc_tp(),
                                    Coord{.lat_=25,.lon_=25},
                                    Organization::ECMWF,
                                    params,
                                    sys_days(year(1989)/month(1)/day(1)),
                                    sys_days(year(1990)/month(1)/day(2)),
                                    DateTimeDiff(err,std::chrono::hours(1)),
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    RepresentationType::GAUSSIAN);
    EXPECT_TRUE(matched_grib1.empty());
    matched_grib1 = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().match(
                                    "any_path.grib"s,
                                    utc_tp(),
                                    Coord{.lat_=25,.lon_=25},
                                    Organization::WMO,
                                    params,
                                    sys_days(year(1989)/month(1)/day(1)),
                                    sys_days(year(1990)/month(1)/day(2)),
                                    DateTimeDiff(err,std::chrono::hours(1)),
                                    TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                                    RepresentationType::GAUSSIAN);
    EXPECT_TRUE(matched_grib1.empty());
}

class DataTestClass_1:public Data,public testing::Test{
    protected:
    std::string fn;
    size_t matched_count_1 = 0;
    std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>> match_result_1;
    std::vector<Organization> centers = {Organization::WMO,Organization::ECMWF,Organization::DWD};
    std::vector<std::pair<Organization,std::vector<uint8_t>>> tables_by_id_ = {{Organization::ECMWF,{228,180,210,130,211,162}},
                                                                            {Organization::WMO,{1,2,3}}};
    std::unordered_set<Grib1CommonDataProperties> cmn_search_1{
                                        {Organization::ECMWF,228,17},
                                        {Organization::ECMWF,211,63},
                                        {Organization::ECMWF,180,3},
                                        {Organization::WMO,1,17}};
    Lat top_1 = 50;
    Lat bottom_1 = 25;
    Lon left_1 = 40;
    Lon right_1 = 45;
    TimeForecast tf_1 = TimeForecast(TimeFrame::HOUR,
                            TimeRangeIndicator::UNINIT_REF_TIME,
                            TimeForecast::period_t{.val=12},
                            TimeForecast::period_t{.val=0});
    Level lvl_1 = Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0);
    utc_tp_t<std::chrono::seconds> from_1 = sys_days(1991y/1/1);
    utc_tp_t<std::chrono::seconds> to_1 = sys_days(2000y/1/1);
    DateTimeDiff diff_1 = [](){
                            std::error_code err;
                            return DateTimeDiff(err,std::chrono::days(1));
                        }();
    public:
    DataTestClass_1():fn("data_file.g1bd"){
        DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1> gribdata;
        grid::GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid;
        grid.base_.dx=1;
        grid.base_.dy=1;
        grid.base_.nx=50;
        grid.base_.ny=50;
        grid.base_.x1=0;
        grid.base_.x2=50;
        grid.base_.y1=50;
        grid.base_.y2=0;
        grid.base_.scan_mode.points_sub_j_dir=true;
        Location<false> any;
        auto time = utc_tp::clock::now();
        std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> msg_data;
        auto err = std::error_code();
        for(int id = 1;id<=tables_by_id_.size();++id){
            uint64_t count = 0;
            any = Location<false>::file("any_path_"s+std::to_string(id)+".grib"s,time);
            for(int d=0;d<=(sys_days(year(1990)/month(id+1)/day(1))-days(1)-sys_days(year(1990)/month(id)/day(1)))/days(1);++d)
            {
                for(auto table:tables_by_id_[id-1].second)
                    for(int param = 16+id;param<130+id;param+=16+id){
                        ptrdiff_t cur_pos = 1000*count++;
                        auto f_error = API::ErrorData::ErrorCode<API_T::GRIB1>::NONE_ERR;
                        auto err = std::error_code();
                        auto msg = data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>(GridInfo(grid),sys_days(year(1990+id)/month(id)/day(1))+days(d),
                                    cur_pos,1000+2000*id,param,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::UNINIT_REF_TIME,
                                        {static_cast<uint8_t>(6+id)},{static_cast<uint8_t>(0)}),
                                    tables_by_id_[id-1].first,table,Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),f_error);
                        // std::cout<<(cmn_search_1.contains(Grib1CommonDataProperties(msg.center,msg.table_version,msg.parameter))?
                        //                 "Contains":"Does not contains")<<std::endl;
                        if(msg.date>=from_1 && msg.date<=to_1 && TimeForecast::compare(TimeForecast::LESS,msg.t_unit,tf_1,err) &&
                            grid.pos_in_grid(Coord(top_1,left_1)) && grid.pos_in_grid(Coord(top_1,right_1)) &&
                            grid.pos_in_grid(Coord(bottom_1,left_1)) && grid.pos_in_grid(Coord(bottom_1,right_1)) &&
                            cmn_search_1.contains(Grib1CommonDataProperties(msg.center,msg.table_version,msg.parameter)) &&
                            Level::compare(Level::COMPARISION_TYPE::EQUAL,msg.level_,lvl_1,err)){
                                Grib1CommonDataProperties cmn(msg.center,msg.table_version,msg.parameter);
                                SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>::Additional_t add;
                                add.fcst_ = msg.t_unit;
                                add.grid_ = msg.grid_data;
                                add.lvl_ = msg.level_;
                                if(auto found = std::find_if(match_result_1.begin(),match_result_1.end(),[&cmn,&add]
                                    (const SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val){
                                        return val.cmn_==cmn && val.add_.fcst_==add.fcst_ && *val.add_.grid_==*add.grid_&&
                                                val.add_.lvl_==add.lvl_;
                                    });found!=match_result_1.end())
                                    found->add_.ts_.push_time(msg.date,err);
                                else{
                                    add.ts_ = TimeSequence(msg.date);
                                    match_result_1.push_back(SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
                                        .add_ = std::move(add),.cmn_=cmn});
                                    ++matched_count_1;
                                }
                            }
                        msg_data.push_back(std::move(msg));
                    }
            }
        }
        gribdata.add_data(any,msg_data,err);
        for(auto id:data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().index_){
            for(auto& [ts,pos]:id->ts_pos_)
                assert(ts.number_of_intervals()==0);
        }
        update_indexing(std::move(gribdata));
        // for(auto id:data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().index_){
        //     for(auto& [ts,pos]:id->ts_pos_)
        //         std::cout<<ts.number_of_intervals()<<std::endl;
        // }
        
        std::ofstream stream(fn,std::ofstream::trunc|std::ofstream::out);
        serialization::serialize_to_file(data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>(),stream);
    }

    ~DataTestClass_1(){
        assert(std::filesystem::remove(fn));
    }
};

TEST_F(DataTestClass_1,FindAllTest){
    read(fn);
    auto& dstruct = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    std::error_code err;
    std::unordered_set<Grib1CommonDataProperties> cmn;
    cmn.emplace(Organization::ECMWF,228,17);
    cmn.emplace(Organization::ECMWF,211,63);
    cmn.emplace(Organization::ECMWF,180,3);
    cmn.emplace(Organization::WMO,1,17);
    std::vector<std::pair<Location<true>,
        std::vector<MessagePositionSizeInfo>>> pos;
    auto matched_data = data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().
            find_all(   pos,
                        cmn,utc_tp(),50,25,40,45,
                        sys_days(1991y/1/1),
                        sys_days(2000y/1/1),
                        DateTimeDiff(err,std::chrono::days(1)),
                        std::make_pair(TimeForecast(TimeFrame::HOUR,
                            TimeRangeIndicator::UNINIT_REF_TIME,
                            TimeForecast::period_t{.val=12},
                            TimeForecast::period_t{.val=0}),TimeForecast::LESS),
                        std::make_pair(Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),Level::EQUAL),
                        std::nullopt);
    EXPECT_EQ(matched_data.size(),matched_count_1);
    for(auto& res:match_result_1)
        EXPECT_TRUE(std::find(matched_data.begin(),matched_data.end(),res)!=matched_data.end());
    // matched_data = find_all(RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR,
    //                     ts,
    //                     TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
    //                     Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
    //                     utc_tp());
    // EXPECT_EQ(matched_data.size(),cmn_sz);
    // EXPECT_EQ(matched_data.at(Location<false>::file("any_path_2.grib"s,utc_tp())).buf_pos_.size(),((sys_days(1990y/2/2)-sys_days(1990y/2/1))/days(1)+1)*2);
    // ASSERT_TRUE(matched_data.at(Location<false>::file("any_path_2.grib"s,utc_tp())).grid_data_.has_value());
    // EXPECT_EQ(matched_data.at(Location<false>::file("any_path_2.grib"s,utc_tp())).grid_data_->index(),1);
    // auto sequence = matched_data.at(Location<false>::file("any_path_2.grib"s,utc_tp())).sequence_time_;
    // std::cout<<"Stored: from "<<sequence.get_interval().from()<<" to "<<sequence.get_interval().to()<<std::endl;
    // EXPECT_EQ(matched_data.at(Location<false>::file("any_path_2.grib"s,utc_tp())).sequence_time_,TimeSequence(sys_days(1990y/2/1),sys_days(1990y/2/2),days(1)));
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}