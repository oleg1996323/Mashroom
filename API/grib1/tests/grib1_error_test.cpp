#include "grib1_error.h"
#include "common/common_error.h"
#include <gtest/gtest.h>

TEST(ContextErrorTest, CommonErrorTest){
    osterlib::ContextedError err(api::errc<API_T::COMMON>::bad_file);
    err.with_context("unvailable file").with_field("file","any.txt");
    EXPECT_EQ("bad file: unavailable file (file=any.txt)",err.what());
}

#ifdef GRIB1API
TEST(ContextErrorTest, Grib1ErrorTest){

}
#endif

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}