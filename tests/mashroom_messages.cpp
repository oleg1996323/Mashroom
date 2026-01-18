#include <gtest/gtest.h>
#include "network/common/message/message_handler.h"
#include "program/data.h"

using namespace network;

TEST(NetworkMesssageHandler,ClientSide){
    MessageHandler<Side::CLIENT> hmsg;
    hmsg.emplace_message<MESSAGE_ID<Side::CLIENT>::SERVER_STATUS>();
    ASSERT_TRUE(hmsg.has_message());
    ASSERT_EQ(hmsg.index(),MESSAGE_ID<Side::CLIENT>::SERVER_STATUS+1);
    hmsg.clear();
    ASSERT_EQ(hmsg.index(),0);
    ASSERT_EQ(hmsg.emplace_default_message_by_id(MESSAGE_ID<Side::CLIENT>::INDEX_REF),ErrorCode::NONE);
    ASSERT_EQ(hmsg.index(),MESSAGE_ID<Side::CLIENT>::INDEX_REF+1);
}

TEST(NetworkMesssageHandler,ServerSide){
    MessageHandler<Side::SERVER> hmsg;
    hmsg.emplace_message<MESSAGE_ID<Side::SERVER>::SERVER_STATUS>();
    ASSERT_TRUE(hmsg.has_message());
    ASSERT_EQ(hmsg.index(),MESSAGE_ID<Side::SERVER>::SERVER_STATUS+1);
    hmsg.clear();
    ASSERT_EQ(hmsg.index(),0);
    
    ASSERT_EQ(hmsg.emplace_default_message_by_id(MESSAGE_ID<Side::SERVER>::DATA_REPLY_INDEX_REF),ErrorCode::NONE);
    ASSERT_EQ(hmsg.index(),MESSAGE_ID<Side::SERVER>::DATA_REPLY_INDEX_REF+1);
}

TEST(NetworkMesssageHandler,MessageHandlerSerializationTest){
    MessageHandler<Side::CLIENT> handler;
    handler.emplace_message_by_id(Client_MsgT::INDEX_REF);
    network::list_message<Side::CLIENT>::type var;
    var = network::Message<Client_MsgT::INDEX_REF>();
    ASSERT_EQ(serialization::serial_size(handler),serialization::serial_size(var));
    std::vector<char> buffer;
    ASSERT_EQ(serialization::serialize_network(handler,buffer),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialization::deserialize_network(var,buffer),serialization::SerializationEC::NONE);
    ASSERT_TRUE(std::holds_alternative<network::Message<Client_MsgT::INDEX_REF>>(var));
}

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
                                    cur_pos,1000+2000*id,param,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    tables_by_id_[id-1].first,table,Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0)));
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

TEST(BaseIndexResult,BaseIndexResultGrib1SerializationTest){
    network::BaseIndexResult<Data_t::TIME_SERIES,Data_f::GRIB_v1> index;
    
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}