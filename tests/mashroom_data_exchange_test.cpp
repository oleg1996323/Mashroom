#include <gtest/gtest.h>
#include "program/data.h"
#include "network/client.h"
#include "network/server.h"
#include <fstream>

class TestingServer:public network::Server{
    public:
    TestingServer(const network::server::Settings& settings):Server(settings){}
};

class DataTestClass:public Data,public testing::Test{
    protected:
    TestingServer server_;
    std::string fn;
    std::vector<ptrdiff_t> pos_;
    std::unordered_set<SearchParamTableVersion> params{ SearchParamTableVersion{.param_=16,.t_ver_=128},
                                                    SearchParamTableVersion{.param_=48,.t_ver_=228}};
    public:
    DataTestClass():fn("data_file.gbd"),server_(network::server::Settings("127.0.0.1","",Protocol::TCP,30,32396,true)){
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
                                cur_pos,1000,param,TimeFrame::HOUR,Organization::ECMWF,table_v));
                    if(params.contains(SearchParamTableVersion{.param_=static_cast<uint8_t>(param),.t_ver_=static_cast<uint8_t>(table_v)}))
                        pos_.push_back(cur_pos);
                }
        }
        sublimed.add_data(gribdata);
        ds->add_data(sublimed);
        std::ofstream stream(fn,std::ofstream::trunc|std::ofstream::out);
        serialization::serialize_to_file(*ds,stream);
        network::server::Settings sets;
        sets.host_ = "127.0.0.1";
        sets.port_ = 32396;
        sets.reuse_address_ = true;
        server_.launch();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ~DataTestClass(){
        assert(std::filesystem::remove(fn));
    }
};

TEST_F(DataTestClass,Index_DataExchangeTest){
    Client client("127.0.0.1",32396);
    auto additional = network::make_additional<Client_MsgT::INDEX_REF>();
    auto& parameters_struct = additional.add_indexation_parameters_structure<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    parameters_struct.forecast_preference_=TimeFrame::HOUR;
    parameters_struct.grid_type_=RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
    parameters_struct.time_ = TimeSequence(utc_tp(),utc_tp::clock::now(),days(1));
    EXPECT_TRUE(client.connect("127.0.0.1",32396).has_socket());
    Message<Client_MsgT::INDEX_REF> msg(std::move(additional));
    auto err = client.request<Client_MsgT::INDEX_REF>(true,std::move(msg));
    EXPECT_EQ(err,ErrorCode::NONE);
    auto& result = client.get_intermediate_result<network::Server_MsgT::DATA_REPLY_INDEX_REF>(30);
    EXPECT_EQ(result.additional().blocks_.size(),1);    
    EXPECT_FALSE(result.message_more());
}

TEST_F(DataTestClass,Extract_DataExchangeTest){
    Client client("127.0.0.1",32396);
    auto additional = network::make_additional<Client_MsgT::DATA_REQUEST>();
    SearchProperties props;
    props.center_=Organization::ECMWF;
    props.fcst_unit_ = TimeFrame::HOUR;
    props.from_date_ = sys_days(1990y/1/1);
    props.to_date_ = utc_tp::clock::now();
    props.grid_type_ = RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
    props.position_ = Coord{.lat_=50.,.lon_=50.};
    additional.form_=std::move(ExtractMeteoGrib(props,std::nullopt,std::nullopt));
    EXPECT_TRUE(client.connect("127.0.0.1",32396).has_socket());
    Message<Client_MsgT::DATA_REQUEST> msg(std::move(additional));
    auto err = client.request<Client_MsgT::DATA_REQUEST>(true,std::move(msg));
    EXPECT_EQ(err,ErrorCode::NONE);
    auto& result = client.get_intermediate_result<network::Server_MsgT::DATA_REPLY_EXTRACT>(30);
    //EXPECT_EQ(result.additional().,1);    
}

int main(int argc,char* argv[]){
    {
        Client client("127.0.0.1",32396);
        auto additional = network::make_additional<Client_MsgT::INDEX_REF>();
        auto& parameters_struct = additional.add_indexation_parameters_structure<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
        parameters_struct.forecast_preference_=TimeFrame::HOUR;
        parameters_struct.grid_type_=RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
        parameters_struct.time_ = TimeSequence(utc_tp(),utc_tp::clock::now(),days(1));
        EXPECT_TRUE(client.connect("127.0.0.1",32396).has_socket());
        Message<Client_MsgT::INDEX_REF> msg(std::move(additional));
        auto err = client.request<Client_MsgT::INDEX_REF>(true,std::move(msg));
        EXPECT_EQ(err,ErrorCode::NONE);
        auto& result = client.get_intermediate_result<network::Server_MsgT::DATA_REPLY_INDEX_REF>(30);
        EXPECT_EQ(result.additional().blocks_.size(),1);    
        EXPECT_FALSE(result.message_more());

        auto additional_extr = network::make_additional<Client_MsgT::DATA_REQUEST>();
        SearchProperties props;
        props.center_=Organization::ECMWF;
        props.fcst_unit_ = TimeFrame::HOUR;
        props.from_date_ = sys_days(1990y/1/1);
        props.to_date_ = utc_tp::clock::now();
        props.grid_type_ = RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
        props.position_ = Coord{.lat_=50.,.lon_=50.};
        additional_extr.form_=std::move(ExtractMeteoGrib(props,std::nullopt,std::nullopt));
        client.connect("127.0.0.1",32396).has_socket();
        Message<Client_MsgT::DATA_REQUEST> msg_extr(std::move(additional_extr));
        err = client.request<Client_MsgT::DATA_REQUEST>(true,std::move(msg_extr));
    }
    // testing::InitGoogleTest(&argc,argv);
    // return RUN_ALL_TESTS();
    return 0;
}