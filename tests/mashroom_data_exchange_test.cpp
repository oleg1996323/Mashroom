#include <gtest/gtest.h>
#include "program/data.h"
#include "web/client.h"
#include "web/server.h"
#include <fstream>
#include "program/mashroom.h"


class DataTestClass:public Data,public testing::Test{
    protected:
    std::string fn;
    std::vector<ptrdiff_t> pos_;
    std::unordered_set<SearchParamTableVersion> params{ SearchParamTableVersion{.param_=16,.t_ver_=128},
                                                    SearchParamTableVersion{.param_=48,.t_ver_=228}};
    public:
    DataTestClass():
    fn("data_file.g1bd")
    {
        network::server::Settings settings_;
        settings_.host_="127.0.0.1";
        settings_.port_=32396;
        settings_.num_threads_pool_=1;
        settings_.options_.reuse_address_={true,{}};
        settings_.options_.reuse_port_={true,{}};
        settings_.options_.keep_alive_={true,{}};
        app().config().server_config().add("test",std::move(settings_));
        app().config().server_config().set_current("test");
        std::error_code err;
        Mashroom::instance().server().configure(app().config().server_config().current_settings(),err);
        if(err!=std::error_code())
            throw std::runtime_error("config error");
        Mashroom::instance().server().launch(err);
        Mashroom::instance().server().set_processes_at_connections<network::ServerConnectionProcess>();
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
        std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> msg_data;
        for(int d=0;d<=(sys_days(year(1990)/month(1)/day(31))-sys_days(year(1990)/month(1)/day(1)))/days(1);++d)
        {
            for(int table_v = 128;table_v<229;table_v+=228-128)
                for(int param = 16;param<130;param+=16){
                    ptrdiff_t cur_pos = 1000*count++;
                    auto f_error = API::ErrorData::ErrorCode<API::TYPES::GRIB1>::NONE_ERR;
                    auto err = std::error_code();
                    data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> msg(GridInfo(grid),sys_days(year(1990)/month(1)/day(1))+days(d),
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
        if(err!=std::error_code())
            throw std::runtime_error("launch error");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ~DataTestClass(){
        assert(std::filesystem::remove(fn));
    }
};

TEST_F(DataTestClass,Index_DataExchangeTest){
    std::error_code err;
    ASSERT_FALSE(err);
    auto additional = network::Message<network::Client_MsgT::INDEX_REF>();
    auto& parameters_struct = additional.add_index<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
    parameters_struct.forecast_preference(TimeForecast(TimeFrame::HOUR,
                        TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                        TimeForecast::LESS);
    parameters_struct.level_preference(Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),
                        Level::EQUAL);
    parameters_struct.grid_type_=RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
    std::error_code ec;
    int32_t intervals = TimeSequence::full_number_of_intervals(
        utc_tp_t<std::chrono::seconds>(),
        std::chrono::floor<std::chrono::seconds>(utc_tp::clock::now()),
        DateTimeDiff(ec,days(1)),ec);
    ASSERT_TRUE(intervals>0);
    TimeSequence ts(utc_tp(),DateTimeDiff(ec,days(1)),uint16_t(intervals));
    ASSERT_EQ(ec,std::error_code());
    {
        auto& ti = ts.get_interval();
        parameters_struct.from_ = ti.from();
        parameters_struct.to_ = ti.to();
    }
    parameters_struct.tdiff_ = ts.time_duration();
    network::ConnectionHandle hconn = Mashroom::instance().connect(ec,
        "127.0.0.1",
        32396,
        app().config().client_config().current_settings());
    EXPECT_TRUE(hconn.is_valid_handler());
    ASSERT_TRUE(hconn.add_process(std::make_unique<network::ClientConnectionProcess>(hconn,ec),ec));
    ASSERT_FALSE(ec);
    network::Message<network::Client_MsgT::INDEX_REF> msg(std::move(additional));
    auto result = Mashroom::instance().request(
        hconn,
        std::monostate(),
        std::move(msg),
        std::monostate());
    EXPECT_TRUE(result->wait_ready(30));
    if(result->error().has_value())
        std::cout<<"Error: "<<result->error().value().message()<<std::endl;
    else {
        if(result->received() && 
            result->received()->data_frame().message_type().has_value() && 
            result->received()->data_frame().message_type().value()==network::Server_MsgT::INDEX)
            std::cout<<"Received INDEX from server. Message: "<<
                to_json(result->received()->data_frame().get_message<network::Server_MsgT::INDEX>().value().get().index_blocks())<<std::endl;
        else std::cout<<"Not received"<<std::endl;
    }
    ASSERT_FALSE(!result->error().has_value());
    auto add_data = [&](auto& block){
        using decay = std::decay_t<decltype(block)>;
        if constexpr(std::is_same_v<decay,std::monostate>){
            assert(true);
        }
        else if constexpr (std::is_same_v<decay,DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::find_all_t>){
            ASSERT_TRUE(true);
            // DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1> d;
            // d.add_data(path::Storage<false>::host(path.path_,path.add_.get<path::TYPE::HOST>().port_,utc_tp::clock::now()),block);
            // Mashroom::instance().data().update_indexing(std::move(d));
        }
        else{
            assert((std::is_same_v<decay,network::AppMsg<network::Side::SERVER>>));
        }
    };
    ASSERT_NE(result->received().get(),nullptr);
    std::visit(add_data,result->received()->data_frame().data());
    // ASSERT_TRUE(res)
    // EXPECT_EQ((std::get<std::vector<SearchDataResult<(Data_t)1U, (Data_f)1>>>(result.additional().blocks_).size()),1);    
    // EXPECT_FALSE(result.message_more());
}

// TEST_F(DataTestClass,Extract_DataExchangeTest){
//     // Client client("127.0.0.1",32396);
//     // auto additional = network::make_additional<Client_MsgT::DATA_REQUEST>();
//     // std::error_code ec;
//     // SearchProperties props;
//     // props.center_=Organization::ECMWF;
//     // props.fcst_unit_ = TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0});
//     // props.from_date_ = sys_days(1990y/1/1);
//     // props.to_date_ = std::chrono::floor<std::chrono::seconds>(utc_tp::clock::now());
//     // props.grid_type_ = RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
//     // props.position_ = Coord{.lat_=50.,.lon_=50.};
//     // additional.form_=std::move(ExtractMeteoGrib(props,std::nullopt,std::nullopt));
//     // EXPECT_TRUE(client.connect("127.0.0.1",32396,ec).has_socket());
//     // Message<Client_MsgT::DATA_REQUEST> msg(std::move(additional));
//     // auto err = client.request<Client_MsgT::DATA_REQUEST>(true,std::move(msg));
//     // EXPECT_EQ(err,ErrorCode::NONE);
//     // auto& result = client.get_intermediate_result<network::Server_MsgT::DATA_REPLY_EXTRACT>(30);
//     // //EXPECT_EQ(result.additional().,1);    
// }

int main(int argc,char* argv[]){
    {
        Mashroom::instance();
        // Client client("127.0.0.1",32396);
        // auto additional = network::make_additional<Client_MsgT::INDEX_REF>();
        // auto& parameters_struct = additional.add_indexation_parameters_structure<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
        // parameters_struct.forecast_preference_=std::make_pair(
        //         TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
        //         TimeForecast::EQUAL);
        // parameters_struct.grid_type_=RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
        // parameters_struct.from_ = utc_tp_t<std::chrono::seconds>();
        // parameters_struct.to_ = std::chrono::floor<std::chrono::seconds>(utc_tp::clock::now());
        // std::error_code error_loc;
        // parameters_struct.tdiff_ = DateTimeDiff(error_loc,days(1));
        // EXPECT_TRUE(client.connect("127.0.0.1",32396,error_loc).has_socket());
        // Message<Client_MsgT::INDEX_REF> msg(std::move(additional));
        // auto err = client.request<Client_MsgT::INDEX_REF>(true,std::move(msg));
        // EXPECT_EQ(err,ErrorCode::NONE);
        // auto& result = client.get_intermediate_result<network::Server_MsgT::DATA_REPLY_INDEX_REF>(30);
        // auto result_check = [&parameters_struct](const auto& block)
        // ->bool
        // {
        //     using type = std::decay_t<decltype(block)>;
        //     if constexpr (std::is_same_v<std::monostate,type>)
        //         return false;
        //     else if constexpr(std::is_same_v<
        //             DataStruct<Data_t::TIME_SERIES,
        //             Data_f::GRIB_v1>::find_all_t,type>){
        //                 return block.size()==1 && block.front().add_.fcst_==
        //                     parameters_struct.forecast_preference_.value().first &&
        //                 block.front().add_.grid_ &&
        //                 block.front().add_.grid_->type()==
        //                 parameters_struct.grid_type_.value()&&
        //                 block.front().add_.ts_.time_duration()==
        //                 parameters_struct.tdiff_.value();
        //             }
        //     else static_assert(false);
        // };
        // EXPECT_TRUE(std::visit(result_check,result.additional().blocks_));
        // // EXPECT_EQ(result.additional().blocks_,1);    
        // // EXPECT_FALSE(result.message_more());

        // // auto additional_extr = network::make_additional<Client_MsgT::DATA_REQUEST>();
        // // SearchProperties props;
        // // props.center_=Organization::ECMWF;
        // // props.fcst_unit_ = TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0});
        // // props.from_date_ = sys_days(1990y/1/1);
        // // props.to_date_ = utc_tp::clock::now();
        // // props.grid_type_ = RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
        // // props.position_ = Coord{.lat_=50.,.lon_=50.};
        // // additional_extr.form_=std::move(ExtractMeteoGrib(props,std::nullopt,std::nullopt));
        // // client.connect("127.0.0.1",32396).has_socket();
        // // Message<Client_MsgT::DATA_REQUEST> msg_extr(std::move(additional_extr));
        // // err = client.request<Client_MsgT::DATA_REQUEST>(true,std::move(msg_extr));
    }
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}