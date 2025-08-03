#include "byte_read.h"
#include "byte_order.h"
#include "float_conv.h"
#include <memory>
#include <cstring>
#include <limits>
#include <gtest/gtest.h>

TEST(BytesTest,FloatToInteger){
    float fp_1 = std::rand()/std::rand();
    auto int_1 = to_integer(fp_1);
    ASSERT_EQ(sizeof(fp_1),sizeof(int_1));
    EXPECT_EQ(std::memcmp(&fp_1,&int_1,sizeof(int_1)),0);
}

TEST(BytesTest,DoubleToLongInteger){
    double fp_1 = std::rand()/std::rand();
    auto int_1 = to_integer(fp_1);
    ASSERT_EQ(sizeof(fp_1),sizeof(int_1));
    EXPECT_EQ(std::memcmp(&fp_1,&int_1,sizeof(int_1)),0);
}

TEST(BytesTest,IntegerToFloat){
    int int_1 = std::rand();
    auto fp_1 = to_float(int_1);
    ASSERT_EQ(sizeof(fp_1),sizeof(int_1));
    EXPECT_EQ(std::memcmp(&fp_1,&int_1,sizeof(int_1)),0);
}

TEST(BytesTest,LongIntegerToDouble){
    long int int_1 = std::rand()+std::numeric_limits<int>::max();
    auto fp_1 = to_float(int_1);
    ASSERT_EQ(sizeof(fp_1),sizeof(int_1));
    EXPECT_EQ(std::memcmp(&fp_1,&int_1,sizeof(int_1)),0);
}

TEST(BytesTest,DetectLittleEndian){
    
}

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}