#include "fixtures.h"
#include <iostream>
#include "proc/extract/write.h"
#include "proc/extract/read.h"
#include "proc/extract/gen.h"

TEST_F(Grib1Fixture,TestSimpleJson){
    auto extracted_data_json = to_json(data());
    auto data_local_result = from_json<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(extracted_data_json);
    ASSERT_TRUE(data_local_result.has_value());
    ExtractedData data_all = data_local_result.value();
    ASSERT_EQ(data_all,data());
}

TEST_F(Grib1Fixture,TestExtractFileJson){
    auto paths = procedures::extract::write_json_file(std::stop_token(),data(),props(),time_period(),"test_generated_files");
    ASSERT_TRUE(paths.size()==1);
    ASSERT_NO_THROW(procedures::extract::read_json_file(std::stop_token(),*paths.begin()));
    auto read_data = procedures::extract::read_json_file(std::stop_token(),*paths.begin());
    ASSERT_EQ(read_data,data());
    fs::remove_all("test_generated_files");
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}