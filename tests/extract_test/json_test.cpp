#include "fixtures.h"

TEST_F(Grib1Fixture,TestJson){
    auto extracted_data_json = to_json(data());
    auto data_local_result = from_json<ExtractedData>(extracted_data_json);
    ASSERT_TRUE(data_local_result.has_value());
    ASSERT_TRUE(data_local_result==data());
}

int main(){
    return 0;
}