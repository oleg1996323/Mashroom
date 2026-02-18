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
    DataTestClass():fn("data_file.g1bd"),server_(network::server::Settings("127.0.0.1","",Protocol::TCP,30,32396,true)){
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
        path::Storage<false> any = path::Storage<false>::file("any_path.grib"s,utc_tp::clock::now());
        uint64_t count = 0;
        std::vector<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> msg_data;
        auto err = std::error_code();
        for(int d=0;d<=(sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1);++d)
        {
            for(int table_v = 128;table_v<229;table_v+=228-128)
                for(int param = 16;param<130;param+=16){
                    ptrdiff_t cur_pos = 1000*count++;
                    auto f_error = API::ErrorData::ErrorCode<API::TYPES::GRIB1>::NONE_ERR;
                    auto err = std::error_code();
                    FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> msg(GridInfo(grid),sys_days(year(1990)/month(1)/day(1))+days(d),
                                cur_pos,1000,param,
                                TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                Organization::ECMWF,table_v,
                                Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),f_error);
                    msg_data.push_back(std::move(msg));
                    if(params.contains(SearchParamTableVersion{.param_=static_cast<uint8_t>(param),.t_ver_=static_cast<uint8_t>(table_v)}))
                        pos_.push_back(cur_pos);
                }
        }
        gribdata.add_data(any,msg_data,err);
        update_indexing(std::move(gribdata));
        std::ofstream stream(fn,std::ofstream::trunc|std::ofstream::out);
        serialization::serialize_to_file(gribdata,stream);
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
    parameters_struct.set_forecast_preference(TimeForecast(TimeFrame::HOUR,
                        TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                        TimeForecast::LESS);
    parameters_struct.set_level_preference(Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                        Level::EQUAL);
    parameters_struct.grid_type_=RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
    std::error_code ec;
    TimeSequence ts(utc_tp(),utc_tp::clock::now(),ec,days(1));
    ASSERT_EQ(ec,std::error_code());
    {
        auto& ti = ts.get_interval();
        parameters_struct.from_ = ti.from();
        parameters_struct.to_ = ti.to();
    }
    parameters_struct.tdiff_ = ts.time_duration();
    EXPECT_TRUE(client.connect("127.0.0.1",32396).has_socket());
    Message<Client_MsgT::INDEX_REF> msg(std::move(additional));
    auto err = client.request<Client_MsgT::INDEX_REF>(true,std::move(msg));
    EXPECT_EQ(err,ErrorCode::NONE);
    auto& result = client.get_intermediate_result<network::Server_MsgT::DATA_REPLY_INDEX_REF>(30);
    ASSERT_TRUE((std::holds_alternative<std::vector<SearchDataResult<Data_t::TIME_SERIES,Data_f::GRIB_v1>>>(result.additional().blocks_)));
    EXPECT_EQ((std::get<std::vector<SearchDataResult<(Data_t)1U, (Data_f)1>>>(result.additional().blocks_).size()),1);    
    EXPECT_FALSE(result.message_more());
}

TEST_F(DataTestClass,Extract_DataExchangeTest){
    Client client("127.0.0.1",32396);
    auto additional = network::make_additional<Client_MsgT::DATA_REQUEST>();
    SearchProperties props;
    props.center_=Organization::ECMWF;
    props.fcst_unit_ = TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0});
    props.from_date_ = sys_days(1990y/1/1);
    props.to_date_ = std::chrono::floor<std::chrono::seconds>(utc_tp::clock::now());
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
        parameters_struct.forecast_preference_=std::make_pair(
                TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                TimeForecast::EQUAL);
        parameters_struct.grid_type_=RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
        parameters_struct.from_ = utc_tp_t<std::chrono::seconds>();
        parameters_struct.to_ = std::chrono::floor<std::chrono::seconds>(utc_tp::clock::now());
        std::error_code error_loc;
        parameters_struct.tdiff_ = DateTimeDiff(error_loc,days(1));
        EXPECT_TRUE(client.connect("127.0.0.1",32396).has_socket());
        Message<Client_MsgT::INDEX_REF> msg(std::move(additional));
        auto err = client.request<Client_MsgT::INDEX_REF>(true,std::move(msg));
        EXPECT_EQ(err,ErrorCode::NONE);
        auto& result = client.get_intermediate_result<network::Server_MsgT::DATA_REPLY_INDEX_REF>(30);
        auto result_check = [&parameters_struct](const auto& block)
        ->bool
        {
            using type = std::decay_t<decltype(block)>;
            if constexpr (std::is_same_v<std::monostate,type>)
                return false;
            else if constexpr(std::is_same_v<
                    DataStruct<Data_t::TIME_SERIES,
                    Data_f::GRIB_v1>::find_all_t,type>){
                        return block.size()==1 && block.front().add_.fcst_==
                            parameters_struct.forecast_preference_.value().first &&
                        block.front().add_.grid_ &&
                        block.front().add_.grid_->type()==
                        parameters_struct.grid_type_.value()&&
                        block.front().add_.ts_.time_duration()==
                        parameters_struct.tdiff_.value();
                    }
            else static_assert(false);
        };
        EXPECT_TRUE(std::visit(result_check,result.additional().blocks_));
        // EXPECT_EQ(result.additional().blocks_,1);    
        // EXPECT_FALSE(result.message_more());

        // auto additional_extr = network::make_additional<Client_MsgT::DATA_REQUEST>();
        // SearchProperties props;
        // props.center_=Organization::ECMWF;
        // props.fcst_unit_ = TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0});
        // props.from_date_ = sys_days(1990y/1/1);
        // props.to_date_ = utc_tp::clock::now();
        // props.grid_type_ = RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
        // props.position_ = Coord{.lat_=50.,.lon_=50.};
        // additional_extr.form_=std::move(ExtractMeteoGrib(props,std::nullopt,std::nullopt));
        // client.connect("127.0.0.1",32396).has_socket();
        // Message<Client_MsgT::DATA_REQUEST> msg_extr(std::move(additional_extr));
        // err = client.request<Client_MsgT::DATA_REQUEST>(true,std::move(msg_extr));
    }
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}