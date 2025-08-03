#include "serialization.h"
#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <utility>

class SerializableItems:public testing::Test{
    protected:
    SerializableItems(){
        double fp_val_1 = std::rand()/std::rand();
        double fp_val_2 = std::rand()/std::rand();
        integer_1=std::rand();
        integer_2=std::rand();
        
        tp_ = std::chrono::system_clock::now();
        dur_ = std::chrono::duration_cast<std::chrono::hours>(std::chrono::seconds(std::rand()));
        fp_unique_ = std::make_unique<double>(std::rand()/std::rand());
        fp_shared_ = std::make_shared<double>(std::rand()/std::rand());
        pair_ = std::make_pair(std::rand()/std::rand(),std::rand());
        for(int i=0;i<100;++i)
            vector_pairs_.push_back(std::make_pair(std::rand()/std::rand(),std::make_shared<int>(std::rand())));
        for(int i=0;i<50;++i)
            list_pairs_.push_back(std::make_pair(std::rand(),std::rand()));
        for(int i=0;i<30;++i)
            deque_tp_.push_back(std::chrono::system_clock::now());
        for(int i=0;i<65;++i)
            set_int_.insert(std::rand());
        for(int i=0;i<65;++i)
            uset_int_.insert(std::rand());
        for(int i=0;i<65;++i)
            map_int_.emplace(std::rand(),std::rand());
        for(int i=0;i<65;++i)
            umap_int_.emplace(std::rand(),std::rand());
    }
    ~SerializableItems() = default;
    double fp_val_1;
    double fp_val_2;
    int integer_1;
    int integer_2;
    std::chrono::system_clock::time_point tp_;
    std::chrono::system_clock::duration dur_;
    std::unique_ptr<double> fp_unique_;
    std::shared_ptr<double> fp_shared_;
    std::pair<double,int> pair_;
    std::vector<std::pair<double,std::shared_ptr<int>>> vector_pairs_;
    std::list<std::pair<int,int>> list_pairs_;
    std::deque<std::chrono::system_clock::time_point> deque_tp_;
    std::set<int> set_int_;
    std::unordered_set<int> uset_int_;
    std::map<int,int> map_int_;
    std::unordered_map<int,int> umap_int_;
};

struct CheckValues{
    double fp_val_1;
    double fp_val_2;
    int integer_1;
    int integer_2;
    std::chrono::system_clock::time_point tp_;
    std::chrono::system_clock::duration dur_;
    std::unique_ptr<double> fp_unique_;
    std::shared_ptr<double> fp_shared_;
    std::pair<double,int> pair_;
    std::vector<std::pair<double,std::shared_ptr<int>>> vector_pairs_;
    std::list<std::pair<int,int>> list_pairs_;
    std::deque<std::chrono::system_clock::time_point> deque_tp_;
    std::set<int> set_int_;
    std::unordered_set<int> uset_int_;
    std::map<int,int> map_int_;
    std::unordered_map<int,int> umap_int_;
};

TEST(Serialization, SerializeInt){
    using namespace serialization;
    std::vector<char> buf;
    int integer_1 = std::rand();
    if(integer_1>0)
        integer_1=-integer_1;
    int integer_2 = std::rand();
    ASSERT_EQ(serialize<true>(integer_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(integer_2,buf),serialization::SerializationEC::NONE);

    int reversed_1=std::byteswap(integer_1);
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(int))==0);
    int reversed_2=std::byteswap(integer_2);
    EXPECT_TRUE(std::memcmp(&reversed_2,buf.data()+sizeof(reversed_1),sizeof(int))==0);

    int check_int_1 = 0;
    int check_int_2 = 0;
    ASSERT_EQ(deserialize<true>(check_int_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(check_int_2,std::span(buf).subspan(serial_size(check_int_1))),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_int_1,integer_1);
    EXPECT_EQ(check_int_2,integer_2);
}

TEST(Serialization, SerializeFloatingPoint){
    using namespace serialization;
    std::vector<char> buf;
    double fp_1 = std::rand()/std::rand();
    double fp_2 = std::rand();
    ASSERT_EQ(serialize<true>(fp_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_2,buf),serialization::SerializationEC::NONE);

    auto reversed_1=std::byteswap(to_integer(fp_1));
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(double))==0);
    auto reversed_2=std::byteswap(to_integer(fp_2));
    EXPECT_TRUE(std::memcmp(&reversed_2,buf.data()+sizeof(reversed_1),sizeof(double))==0);

    double check_fp_1 = 0;
    double check_fp_2 = 0;
    ASSERT_EQ(deserialize<true>(check_fp_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(check_fp_2,std::span(buf).subspan(serial_size(check_fp_1))),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_fp_1,fp_1);
    EXPECT_EQ(check_fp_2,fp_2);
}

TEST(Serialization, SerializeTimePoint){
    using namespace serialization;
    std::vector<char> buf;
    std::chrono::system_clock::time_point utc_tp = std::chrono::system_clock::now();
    ASSERT_EQ(serialize<true>(utc_tp,buf),serialization::SerializationEC::NONE);

    auto reversed_1=std::byteswap(utc_tp.time_since_epoch().count());
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(reversed_1))==0);

    std::chrono::system_clock::time_point check_tp{};
    ASSERT_EQ(deserialize<true>(check_tp,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_tp,utc_tp);
}

TEST(Serialization, SerializeDuration){
    using namespace serialization;
    std::vector<char> buf;
    std::chrono::system_clock::duration utc_tp = (std::chrono::system_clock::now()-std::chrono::years(20)-std::chrono::months(50)).time_since_epoch();
    ASSERT_EQ(serialize<true>(utc_tp,buf),serialization::SerializationEC::NONE);

    auto reversed_1=std::byteswap(utc_tp.count());
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(reversed_1))==0);

    std::chrono::system_clock::duration check_tp{};
    ASSERT_EQ(deserialize<true>(check_tp,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_tp,utc_tp);
}

TEST(Serialization, SerializeUniquePtr){
    using namespace serialization;
    std::vector<char> buf;
    std::unique_ptr<double> fp_unique_ = std::make_unique<double>(std::rand()/std::rand());
    ASSERT_EQ(serialize<true>(fp_unique_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(fp_unique_));
    auto reversed_1=std::byteswap(to_integer(*fp_unique_.get()));
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(reversed_1))==0);

    std::unique_ptr<double> check_tp{};
    ASSERT_EQ(deserialize<true>(check_tp,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_tp,fp_unique_);

    fp_unique_.reset();
    buf.clear();
    ASSERT_EQ(serialize<true>(fp_unique_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(fp_unique_));

    check_tp.reset();
    ASSERT_EQ(deserialize<true>(check_tp,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_tp,fp_unique_);
    EXPECT_FALSE(check_tp);
}


TEST_F(SerializableItems,SerializeNetworkBytesOrder){
    using namespace serialization;
    std::vector<char> buf;
    /*network bytes-order serialization*/
    ASSERT_EQ(serialize<true>(fp_val_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_val_2,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(integer_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(integer_2,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(tp_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(dur_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_unique_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_shared_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(pair_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(vector_pairs_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(list_pairs_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(deque_tp_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(set_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(uset_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(map_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(umap_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_val_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_val_2,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(integer_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(integer_2,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(tp_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(dur_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_unique_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_shared_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(pair_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(vector_pairs_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(list_pairs_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(deque_tp_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(set_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(uset_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(map_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(umap_int_,buf),serialization::SerializationEC::NONE);

    CheckValues values;

    /*network bytes-order deserialization*/
    ASSERT_EQ(deserialize<true>(values.fp_val_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.fp_val_2,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.integer_1,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.integer_2,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.tp_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.dur_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.fp_unique_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.fp_shared_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.pair_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.vector_pairs_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.list_pairs_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.deque_tp_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.set_int_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.uset_int_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.map_int_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.umap_int_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.fp_val_1,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.fp_val_2,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.integer_1,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.integer_2,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.tp_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.dur_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.fp_unique_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.fp_shared_,std::span(buf).subspan(serial_size(values.fp_val_1))),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.pair_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.vector_pairs_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.list_pairs_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.deque_tp_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.set_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.uset_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.map_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(values.umap_int_,std::span(buf)),serialization::SerializationEC::NONE);
}

TEST_F(SerializableItems,SerializeNativeBytesOrder){
    using namespace serialization;
    std::vector<char> buf;
    /*native bytes-order serialization*/
    ASSERT_EQ(serialize<false>(fp_val_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(fp_val_2,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(integer_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(integer_2,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(tp_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(dur_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(fp_unique_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(fp_shared_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(pair_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(vector_pairs_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(list_pairs_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(deque_tp_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(set_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(uset_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(map_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(umap_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(fp_val_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(fp_val_2,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(integer_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(integer_2,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(tp_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(dur_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(fp_unique_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(fp_shared_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(pair_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(vector_pairs_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(list_pairs_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(deque_tp_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(set_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(uset_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(map_int_,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<false>(umap_int_,buf),serialization::SerializationEC::NONE);

    CheckValues values;

    /*network bytes-order deserialization*/
    ASSERT_EQ(deserialize<false>(values.fp_val_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.fp_val_2,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.integer_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.integer_2,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.tp_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.dur_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.fp_unique_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.fp_shared_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.pair_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.vector_pairs_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.list_pairs_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.deque_tp_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.set_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.uset_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.map_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.umap_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.fp_val_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.fp_val_2,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.integer_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.integer_2,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.tp_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.dur_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.fp_unique_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.fp_shared_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.pair_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.vector_pairs_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.list_pairs_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.deque_tp_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.set_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.uset_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.map_int_,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<false>(values.umap_int_,std::span(buf)),serialization::SerializationEC::NONE);
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}