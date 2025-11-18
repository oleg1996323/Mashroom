#include <gtest/gtest.h>
#include "proc/index/gen.h"
#include "proc/index/write.h"

TEST(Index,input_to_token_sequence_test){
    ASSERT_EQ(index_gen::input_to_token_sequence("/year/month/day/lat/lon"),(std::vector<DIR_TREE_TOKEN>{DIR_TREE_TOKEN::YEAR,DIR_TREE_TOKEN::MONTH,DIR_TREE_TOKEN::DAY,DIR_TREE_TOKEN::LATITUDE,DIR_TREE_TOKEN::LONGITUDE}));
    ASSERT_EQ(index_gen::input_to_token_sequence("/"),(std::vector<DIR_TREE_TOKEN>{}));
    ASSERT_ANY_THROW(ASSERT_EQ(index_gen::input_to_token_sequence("//"),(std::vector<DIR_TREE_TOKEN>{})););
    ASSERT_ANY_THROW(ASSERT_EQ(index_gen::input_to_token_sequence("/long/latit"),(std::vector<DIR_TREE_TOKEN>{})););
}

TEST(Index,check_format_tokens_test){
    ASSERT_TRUE(index_gen::check_format(std::vector<DIR_TREE_TOKEN>{DIR_TREE_TOKEN::YEAR,DIR_TREE_TOKEN::MONTH,DIR_TREE_TOKEN::DAY,DIR_TREE_TOKEN::LATITUDE,DIR_TREE_TOKEN::LONGITUDE}));
    ASSERT_FALSE(index_gen::check_format(std::vector<DIR_TREE_TOKEN>{DIR_TREE_TOKEN::YEAR,DIR_TREE_TOKEN::MONTH,DIR_TREE_TOKEN::DAY,DIR_TREE_TOKEN::LATITUDE,DIR_TREE_TOKEN::LATITUDE}));
    ASSERT_TRUE(index_gen::check_format(std::vector<DIR_TREE_TOKEN>{DIR_TREE_TOKEN::YEAR,DIR_TREE_TOKEN::MONTH,DIR_TREE_TOKEN::DAY,DIR_TREE_TOKEN::LATLON,DIR_TREE_TOKEN::LATITUDE}));
    ASSERT_TRUE(index_gen::check_format(std::vector<DIR_TREE_TOKEN>{DIR_TREE_TOKEN::YEAR,DIR_TREE_TOKEN::MONTH,DIR_TREE_TOKEN::DAY,DIR_TREE_TOKEN::LATLON,DIR_TREE_TOKEN::LONGITUDE}));
    ASSERT_TRUE(index_gen::check_format(std::vector<DIR_TREE_TOKEN>{}));
}

TEST(Index,check_format_string_test){
    ASSERT_TRUE(index_gen::check_format("/year/month/day/lat/lon"));
    ASSERT_TRUE(index_gen::check_format("/year/month/day/latlon/lon"));
    ASSERT_FALSE(index_gen::check_format("/year/month/day/day/lon"));
}

TEST(Index,generate_filename_format_test){
    ASSERT_EQ(index_gen::generate_filename_format(index_gen::input_to_token_sequence("/year/month/day/lat/lon")),index_gen::format(DIR_TREE_TOKEN::YEAR)+"_"+index_gen::format(DIR_TREE_TOKEN::MONTH)+
                                                            "_"+index_gen::format(DIR_TREE_TOKEN::DAY)+"_"+
                                                            index_gen::format(DIR_TREE_TOKEN::LATITUDE)+"_"+
                                                            index_gen::format(DIR_TREE_TOKEN::LONGITUDE));
}

using namespace std::string_literals;

TEST(Index,generate_filename_test){
    ASSERT_EQ(index_gen::generate_filename(IndexOutputFileFormat::JSON,1991y,month(2),1d,25.0f,25.0f),"1991_02_01_25_25.json"s);
}

TEST(Index,generate_directory_format_test){

}

TEST(Index,generate_directory_test){

}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}