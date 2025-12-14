#include "proc/extract/write.h"
#include "proc/extract/read.h"
#include <gtest/gtest.h>
#include "fixtures.h"

TEST_F(Grib1Fixture,extract_test){
    std::stop_token token;
    procedures::extract::write_bin_file(token,data(),props(),time_period(),"test_path.bin","","{}");
}

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}