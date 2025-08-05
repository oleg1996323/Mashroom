#include "include/data/sublimed_info.h"
#include <gtest/gtest.h>

TEST(Serialization, SublimedInfo){
    using namespace serialization;
    std::vector<char> buf;
    SublimedDataInfo data{.grid_data_=std::nullopt,.buf_pos_.insert_range(std::ranges::fill_n())};
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


int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}