#include "cmd_parse/types_parse/array_parse.h"
#include <gtest/gtest.h>

TEST(ParseUserInput,ArrayParse){
    using namespace parse;
    {
        std::string check_1 = {"[any,input,processed]"};
        using type = Array<std::string>;
        type array;
        auto result = type::parse(check_1);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value().data().size(),3);
    }
    {
        std::string check_2 = {"[1,2,3]"};
        using type = Array<int>;
        type array;
        auto result = type::parse(check_2);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(result.value().data().size(),3);
    }
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}