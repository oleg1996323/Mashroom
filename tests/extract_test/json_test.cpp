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
    auto paths = procedures::extract::write_json_file(std::stop_token(),data(),props(),time_period(),"",generate_directory_format(time_period()),
    generate_filename_format(props().center_,props().position_));
    ASSERT_TRUE(paths.empty());
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}