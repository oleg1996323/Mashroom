#include <gtest/gtest.h>
#include "web/common/message_handler.h"
#include "program/data.h"

using namespace network;

TEST(NetworkMesssageHandler,ClientSide){
    MessageHandler<Side::CLIENT> hmsg;
    hmsg.emplace_message<MESSAGE_ID<Side::CLIENT>::SERVER_STATUS>();
    ASSERT_TRUE(hmsg.has_message());
    ASSERT_TRUE(hmsg.message_type().has_value());
    ASSERT_EQ(*hmsg.message_type(),MESSAGE_ID<Side::CLIENT>::SERVER_STATUS);
    hmsg.clear();
    hmsg.emplace_message<Client_MsgT::INDEX_REF>();
}

TEST(NetworkMesssageHandler,ServerSide){
    MessageHandler<Side::SERVER> hmsg;
    hmsg.emplace_message<MESSAGE_ID<Side::SERVER>::SERVER_STATUS>();
    ASSERT_TRUE(hmsg.has_message());
    auto type = hmsg.message_type();
    ASSERT_TRUE(type.has_value());
    ASSERT_EQ(*type,Server_MsgT::SERVER_STATUS);
    hmsg.clear();
    ASSERT_FALSE(hmsg.has_message());
    type = hmsg.message_type();
    ASSERT_FALSE(type.has_value());    
}

TEST(NetworkMesssageHandler,MessageHandlerSerializationTest){
    MessageHandler<Side::CLIENT> send_;
    MessageHandler<Side::CLIENT> recv_;
    auto variants_test = [&]<size_t ENUM>(){
        constexpr Client_MsgT::type ID = static_cast<Client_MsgT::type>(ENUM);
        send_.emplace_message<ID>();
        ASSERT_TRUE(send_.has_message());
        ASSERT_EQ(send_.message_type().value(),ID);
        std::vector<char> buffer;
        ASSERT_EQ(serialization::serialize_network(send_,buffer),serialization::SerializationEC::NONE);
        serialization::StreamSerializer ser;
        ser.push_view(buffer);
        ASSERT_EQ(serialization::deserialize_network(recv_,ser),serialization::SerializationEC::NONE);
        ASSERT_TRUE(recv_.has_message());
        ASSERT_EQ(recv_.message_type().value(),ID);
        ASSERT_EQ(recv_.get_message<ID>()->get(),send_.get_message<ID>()->get());
        send_.clear();
        recv_.clear();
    };
    auto test = [&]<size_t... SZ>(std::index_sequence<SZ...> idx){
        (variants_test.template operator()<SZ>(),...);
    };
    test(std::make_index_sequence<MESSAGE_ID<Side::CLIENT>::msg_number()>());
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
                        API::ErrorData::ErrorCode<API_T::GRIB1> f_error;
                        std::error_code err;
                        auto msg = data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>(GridInfo(grid),sys_days(year(1990)/month(id)/day(1))+days(d),
                                    cur_pos,1000+2000*id,param,TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0}),
                                    tables_by_id_[id-1].first,table,Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0),f_error);
                        msg_data.push_back(std::move(msg));
                    }
            }
            gribdata.add_data(any,msg_data,err);
        }
        update_indexing(std::move(gribdata));
        for(auto& [fn,index_structs]:gribdata.paths_)
            cmn_sz+=index_structs.size();
        std::ofstream stream(fn,std::ofstream::trunc|std::ofstream::out);
        serialization::serialize_to_file(gribdata,stream);
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