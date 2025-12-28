#include "fixtures.h"
#include <iostream>

TEST_F(Grib1Fixture,TestJson){
    auto extracted_data_json = to_json(data());
    auto data_local_result = from_json<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(extracted_data_json);
    ASSERT_TRUE(data_local_result.has_value());
    ExtractedData data_all = data_local_result.value();
    std::cout<<"First:"<<"\n"<<extracted_data_json<<std::endl;
    std::cout<<"Second:"<<"\n"<<to_json(data_all);
    ASSERT_EQ(data_all,data());
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}