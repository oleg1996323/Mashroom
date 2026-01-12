#include "proc/extract/write.h"
#include "proc/extract/read.h"
#include <gtest/gtest.h>
#include "proc/extract/gen.h"
#include "fixtures.h"

TEST_F(Grib1Fixture,extract_test){
    try{
        auto paths = procedures::extract::write_bin_file(std::stop_token(),data(),props(),time_period(),"test_generated_files");
        ASSERT_EQ(paths.size(),1);
        auto read_data = procedures::extract::read_bin_file(std::stop_token(),*paths.begin());
        ASSERT_EQ(read_data,data());
        fs::remove_all("test_generated_files");
    }
    catch(...){
        ASSERT_TRUE(false);
    }
}

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}