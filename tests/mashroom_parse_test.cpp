#include "cmd_parse/types_parse/array_parse.h"
#include <gtest/gtest.h>
#include "types_parse/timeval.h"
#include "types_parse/linger.h"
#include "types_parse/datetimediff_parse.h"

TEST(ParseUserInput,ArrayParse){
    using namespace parse;
    {
        std::string check_1 = {"[\"any\",\"input\",\"processed\"]"};
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

bool operator==(timeval lhs,timeval rhs) noexcept{
    return lhs.tv_sec==rhs.tv_sec &&
        lhs.tv_usec==rhs.tv_usec;
}

TEST(ConversionTest,Timeval){
    timeval t{.tv_sec=1,.tv_usec=200};
    std::string t_str = boost::lexical_cast<std::string>(t);
    timeval other=boost::lexical_cast<timeval>(t_str);
    ASSERT_TRUE(t==other);

    std::string input="1,1212343434";
    timeval parsed = boost::lexical_cast<timeval>(input);
    EXPECT_EQ(parsed,(timeval{.tv_sec=1,.tv_usec=121}));
    input="1.99999";
    parsed = boost::lexical_cast<timeval>(input);
    EXPECT_EQ(parsed,(timeval{.tv_sec=1,.tv_usec=999}));
    input="1l99999";
    EXPECT_THROW(boost::lexical_cast<timeval>(input),std::runtime_error);
}

bool operator==(linger lhs,linger rhs) noexcept{
    return lhs.l_linger==rhs.l_linger &&
        lhs.l_onoff==rhs.l_onoff;
}

TEST(ConversionTest,Linger){
    linger t{.l_onoff=true,.l_linger=200};
    std::string t_str = boost::lexical_cast<std::string>(t);
    linger other=boost::lexical_cast<linger>(t_str);
    ASSERT_TRUE(t==other);

    std::string input="1212343";
    linger parsed = boost::lexical_cast<linger>(input);
    EXPECT_EQ(parsed,(linger{.l_onoff=true,.l_linger=1212343}));
    input="99999";
    parsed = boost::lexical_cast<linger>(input);
    EXPECT_EQ(parsed,(linger{.l_onoff=true,.l_linger=99999}));
    input="1l99999";
    EXPECT_THROW(boost::lexical_cast<linger>(input),std::runtime_error);
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}