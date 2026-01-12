#include <gtest/gtest.h>
#include "fixtures.h"
#include "proc/extract/write.h"
#include "proc/extract/read.h"
#include "proc/extract/gen.h"

TEST_F(Grib1Fixture,TestExtractFileTxt){
    auto paths = procedures::extract::write_txt_file(std::stop_token(),data(),props(),time_period(),"test_generated_files");
    ASSERT_TRUE(paths.size()==1);
    ASSERT_NO_THROW(procedures::extract::read_txt_file(std::stop_token(),*paths.begin()));
    auto read_data = procedures::extract::read_txt_file(std::stop_token(),*paths.begin());
    auto other = procedures::extract::write_txt_file(std::stop_token(),read_data,props(),time_period(),"test_comparing_generated_files");
    auto compare = [&read_data](const auto& data_1){
        auto compare_1 = [&data_1](const auto& data_2){
            for(auto& [common,values]:data_2){
                ASSERT_TRUE(data_1.contains(common));
                for(int i=0;i<values.size();++i){
                    ASSERT_EQ(values.at(i),data_1.at(common).at(i));
                }
            }
        };
        std::visit(compare_1,read_data);
    };
    std::visit(compare,data());
    ASSERT_EQ(read_data,data());

    fs::remove_all("test_generated_files");
    fs::remove_all("test_comparing_generated_files");
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}