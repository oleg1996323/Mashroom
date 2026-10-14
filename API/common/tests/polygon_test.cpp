#include "common/geometry/Polygon.h"
#include "common/geometry/Point.h"
#include <gtest/gtest.h>

TEST(OsterLib_polygon,cut_test_counter){
    {
        Line line = {
            {.lat_=21, .lon_=7},
            {.lat_=-1, .lon_=7}
        };
        {           
            {
                Polygon pg;
                pg.append({.lat_=0,.lon_=0});
                pg.append({.lat_=0,.lon_=10});
                pg.append({.lat_=3,.lon_=10});
                pg.append({.lat_=3,.lon_=5});
                pg.append({.lat_=5.5,.lon_=5});
                pg.append({.lat_=5.5,.lon_=10});
                pg.append({.lat_=5,.lon_=10});
                pg.append({.lat_=5,.lon_=6});
                pg.append({.lat_=4,.lon_=6});
                pg.append({.lat_=4,.lon_=11});
                pg.append({.lat_=7,.lon_=11});
                pg.append({.lat_=9,.lon_=10});
                pg.append({.lat_=9,.lon_=6.5});
                pg.append({.lat_=10,.lon_=6.5});
                pg.append({.lat_=10,.lon_=8});
                pg.append({.lat_=10.5,.lon_=8});
                pg.append({.lat_=10.5,.lon_=6});
                pg.append({.lat_=8,.lon_=6});
                pg.append({.lat_=8,.lon_=9});
                pg.append({.lat_=5.75,.lon_=9});
                pg.append({.lat_=5.75,.lon_=8});
                pg.append({.lat_=6.75,.lon_=8});
                pg.append({.lat_=6.75,.lon_=6.75});
                pg.append({.lat_=6.5,.lon_=6.75});
                pg.append({.lat_=6.5,.lon_=7.5});
                pg.append({.lat_=6,.lon_=7.5});
                pg.append({.lat_=6,.lon_=6.25});
                pg.append({.lat_=6.25,.lon_=6.25});
                pg.append({.lat_=6.25,.lon_=7.25});
                pg.append({.lat_=6.375,.lon_=7.25});
                pg.append({.lat_=6.375,.lon_=6.5});
                pg.append({.lat_=7,.lon_=6.5});
                pg.append({.lat_=7,.lon_=3});
                pg.append({.lat_=8,.lon_=3});
                pg.append({.lat_=12,.lon_=7});
                pg.append({.lat_=12,.lon_=10});
                pg.append({.lat_=16,.lon_=10});
                pg.append({.lat_=16,.lon_=5});
                pg.append({.lat_=18,.lon_=5});
                pg.append({.lat_=18,.lon_=10});
                pg.append({.lat_=20,.lon_=10});
                pg.append({.lat_=20,.lon_=3});
                pg.append({.lat_=14,.lon_=3});
                pg.append({.lat_=14,.lon_=8});
                pg.append({.lat_=13,.lon_=8});
                pg.append({.lat_=13,.lon_=0});
                pg.append({.lat_=0,.lon_=0});
                ASSERT_TRUE(pg.is_valid());
                //print(pg);
                std::cout<<std::endl;
                {
                    std::cout<<"test #1"<<std::endl;
                    auto res = pg.cut(line,pg, true);
                    ASSERT_FALSE(res.empty());
                    for(const Polygon& polyg:res)
                        print(polyg);
                    std::vector<Polygon> etalon = {
                        [](){
                            Polygon result;
                            result.append({.lat_=9,.lon_=7});
                            result.append({.lat_=9,.lon_=10});
                            result.append({.lat_=7,.lon_=11});
                            result.append({.lat_=4,.lon_=11});
                            result.append({.lat_=4,.lon_=7});
                            result.append({.lat_=5,.lon_=7});
                            result.append({.lat_=5,.lon_=10});
                            result.append({.lat_=5.5,.lon_=10});
                            result.append({.lat_=5.5,.lon_=7});
                            result.append({.lat_=6,.lon_=7});
                            result.append({.lat_=6,.lon_=7.5});
                            result.append({.lat_=6.5,.lon_=7.5});
                            result.append({.lat_=6.5,.lon_=7});
                            result.append({.lat_=6.75,.lon_=7});
                            result.append({.lat_=6.75,.lon_=8});
                            result.append({.lat_=5.75,.lon_=8});
                            result.append({.lat_=5.75,.lon_=9});
                            result.append({.lat_=8,.lon_=9});
                            result.append({.lat_=8,.lon_=7});
                            result.append({.lat_=9,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=6.375,.lon_=7});
                            result.append({.lat_=6.375,.lon_=7.25});
                            result.append({.lat_=6.25,.lon_=7.25});
                            result.append({.lat_=6.25,.lon_=7});
                            result.append({.lat_=6.375,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=10.5,.lon_=7});
                            result.append({.lat_=10.5,.lon_=8});
                            result.append({.lat_=10,.lon_=8});
                            result.append({.lat_=10,.lon_=7});
                            result.append({.lat_=10.5,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=16,.lon_=7});
                            result.append({.lat_=16,.lon_=10});
                            result.append({.lat_=12,.lon_=10});
                            result.append({.lat_=12,.lon_=7});
                            result.append({.lat_=13,.lon_=7});
                            result.append({.lat_=13,.lon_=8});
                            result.append({.lat_=14,.lon_=8});
                            result.append({.lat_=14,.lon_=7});
                            result.append({.lat_=16,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=20,.lon_=7});
                            result.append({.lat_=20,.lon_=10});
                            result.append({.lat_=18,.lon_=10});
                            result.append({.lat_=18,.lon_=7});
                            result.append({.lat_=20,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=3,.lon_=7});
                            result.append({.lat_=3,.lon_=10});
                            result.append({.lat_=0,.lon_=10});
                            result.append({.lat_=0,.lon_=7});
                            result.append({.lat_=3,.lon_=7});
                            return result;
                        }()
                    };
                    int counter = 0;
                    for(auto& new_pg:res){
                        if(auto found = std::find_if(etalon.begin(),etalon.end(),
                            [&new_pg](const Polygon& pg){
                            return pg.equivalent(new_pg,false);
                        });found==etalon.end()){
                            std::cout<<"polygon №: "<<counter<<std::endl;
                            //assert(bg::is_valid(new_pg));
                            print(new_pg);
                            EXPECT_TRUE(false);
                        }
                        else EXPECT_TRUE(true);
                        ++counter;
                    }
                }
                {
                    std::cout<<"test #2"<<std::endl;
                    auto res = pg.cut(line,pg, false);
                    ASSERT_FALSE(res.empty());
                    for(const Polygon& polyg:res)
                        print(polyg);
                    std::vector<Polygon> etalon = {
                        [](){
                            Polygon result;
                            result.append({.lat_=20,.lon_=7});
                            result.append({.lat_=20,.lon_=3});
                            result.append({.lat_=14,.lon_=3});
                            result.append({.lat_=14,.lon_=7});
                            result.append({.lat_=16,.lon_=7});
                            result.append({.lat_=16,.lon_=5});
                            result.append({.lat_=18,.lon_=5});
                            result.append({.lat_=18,.lon_=7});
                            result.append({.lat_=20,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=10.5,.lon_=7});
                            result.append({.lat_=10.5,.lon_=6});
                            result.append({.lat_=8,.lon_=6});
                            result.append({.lat_=8,.lon_=7});
                            result.append({.lat_=9,.lon_=7});
                            result.append({.lat_=9,.lon_=6.5});
                            result.append({.lat_=10,.lon_=6.5});
                            result.append({.lat_=10,.lon_=7});
                            result.append({.lat_=10.5,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=6.75,.lon_=6.75});
                            result.append({.lat_=6.5,.lon_=6.75});
                            result.append({.lat_=6.5,.lon_=7});
                            result.append({.lat_=6.75,.lon_=7});
                            result.append({.lat_=6.75,.lon_=6.75});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=5,.lon_=6});
                            result.append({.lat_=4,.lon_=6});
                            result.append({.lat_=4,.lon_=7});
                            result.append({.lat_=5,.lon_=7});
                            result.append({.lat_=5,.lon_=6});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=0,.lon_=0});
                            result.append({.lat_=0,.lon_=7});
                            result.append({.lat_=3,.lon_=7});
                            result.append({.lat_=3,.lon_=5});
                            result.append({.lat_=5.5,.lon_=5});
                            result.append({.lat_=5.5,.lon_=7});
                            result.append({.lat_=6,.lon_=7});
                            result.append({.lat_=6,.lon_=6.25});
                            result.append({.lat_=6.25,.lon_=6.25});
                            result.append({.lat_=6.25,.lon_=7});
                            result.append({.lat_=6.375,.lon_=7});
                            result.append({.lat_=6.375,.lon_=6.5});
                            result.append({.lat_=7,.lon_=6.5});
                            result.append({.lat_=7,.lon_=3});
                            result.append({.lat_=8,.lon_=3});
                            result.append({.lat_=12,.lon_=7});
                            result.append({.lat_=13,.lon_=7});
                            result.append({.lat_=13,.lon_=0});
                            result.append({.lat_=0,.lon_=0});
                            return result;
                        }()
                    };
                    int counter = 0;
                    for(auto& new_pg:res){
                        if(auto found = std::find_if(etalon.begin(),etalon.end(),
                            [&new_pg](const Polygon& pg){
                            return pg.equivalent(new_pg,false);
                        });found==etalon.end()){
                            std::cout<<"polygon №: "<<counter<<std::endl;
                            print(new_pg);
                            EXPECT_TRUE(false);
                        }
                        else EXPECT_TRUE(true);
                        ++counter;
                    }
                }
            }
            //////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////
            /////////////CLOCKWISE////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////
            {
                Polygon pg;
                pg.append({.lat_=0,.lon_=0});
                pg.append({.lat_=0,.lon_=10});
                pg.append({.lat_=3,.lon_=10});
                pg.append({.lat_=3,.lon_=5});
                pg.append({.lat_=5.5,.lon_=5});
                pg.append({.lat_=5.5,.lon_=10});
                pg.append({.lat_=5,.lon_=10});
                pg.append({.lat_=5,.lon_=6});
                pg.append({.lat_=4,.lon_=6});
                pg.append({.lat_=4,.lon_=11});
                pg.append({.lat_=7,.lon_=11});
                pg.append({.lat_=9,.lon_=10});
                pg.append({.lat_=9,.lon_=6.5});
                pg.append({.lat_=10,.lon_=6.5});
                pg.append({.lat_=10,.lon_=8});
                pg.append({.lat_=10.5,.lon_=8});
                pg.append({.lat_=10.5,.lon_=6});
                pg.append({.lat_=8,.lon_=6});
                pg.append({.lat_=8,.lon_=9});
                pg.append({.lat_=5.75,.lon_=9});
                pg.append({.lat_=5.75,.lon_=8});
                pg.append({.lat_=6.75,.lon_=8});
                pg.append({.lat_=6.75,.lon_=6.75});
                pg.append({.lat_=6.5,.lon_=6.75});
                pg.append({.lat_=6.5,.lon_=7.5});
                pg.append({.lat_=6,.lon_=7.5});
                pg.append({.lat_=6,.lon_=6.25});
                pg.append({.lat_=6.25,.lon_=6.25});
                pg.append({.lat_=6.25,.lon_=7.25});
                pg.append({.lat_=6.375,.lon_=7.25});
                pg.append({.lat_=6.375,.lon_=6.5});
                pg.append({.lat_=7,.lon_=6.5});
                pg.append({.lat_=7,.lon_=3});
                pg.append({.lat_=8,.lon_=3});
                pg.append({.lat_=12,.lon_=7});
                pg.append({.lat_=12,.lon_=10});
                pg.append({.lat_=16,.lon_=10});
                pg.append({.lat_=16,.lon_=5});
                pg.append({.lat_=18,.lon_=5});
                pg.append({.lat_=18,.lon_=10});
                pg.append({.lat_=20,.lon_=10});
                pg.append({.lat_=20,.lon_=3});
                pg.append({.lat_=14,.lon_=3});
                pg.append({.lat_=14,.lon_=8});
                pg.append({.lat_=13,.lon_=8});
                pg.append({.lat_=13,.lon_=0});
                pg.append({.lat_=0,.lon_=0});
                ASSERT_TRUE(pg.is_valid());
                //print(pg);
                {
                    std::cout<<"test #3"<<std::endl;
                    auto res = pg.cut(line,pg, true);
                    ASSERT_FALSE(res.empty());
                    for(const Polygon& polyg:res)
                        print(polyg);
                    std::vector<Polygon> etalon = {
                        [](){
                            Polygon result;
                            result.append({.lat_=9,.lon_=7});
                            result.append({.lat_=9,.lon_=10});
                            result.append({.lat_=7,.lon_=11});
                            result.append({.lat_=4,.lon_=11});
                            result.append({.lat_=4,.lon_=7});
                            result.append({.lat_=5,.lon_=7});
                            result.append({.lat_=5,.lon_=10});
                            result.append({.lat_=5.5,.lon_=10});
                            result.append({.lat_=5.5,.lon_=7});
                            result.append({.lat_=6,.lon_=7});
                            result.append({.lat_=6,.lon_=7.5});
                            result.append({.lat_=6.5,.lon_=7.5});
                            result.append({.lat_=6.5,.lon_=7});
                            result.append({.lat_=6.75,.lon_=7});
                            result.append({.lat_=6.75,.lon_=8});
                            result.append({.lat_=5.75,.lon_=8});
                            result.append({.lat_=5.75,.lon_=9});
                            result.append({.lat_=8,.lon_=9});
                            result.append({.lat_=8,.lon_=7});
                            result.append({.lat_=9,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=6.375,.lon_=7});
                            result.append({.lat_=6.375,.lon_=7.25});
                            result.append({.lat_=6.25,.lon_=7.25});
                            result.append({.lat_=6.25,.lon_=7});
                            result.append({.lat_=6.375,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=10.5,.lon_=7});
                            result.append({.lat_=10.5,.lon_=8});
                            result.append({.lat_=10,.lon_=8});
                            result.append({.lat_=10,.lon_=7});
                            result.append({.lat_=10.5,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=16,.lon_=7});
                            result.append({.lat_=16,.lon_=10});
                            result.append({.lat_=12,.lon_=10});
                            result.append({.lat_=12,.lon_=7});
                            result.append({.lat_=13,.lon_=7});
                            result.append({.lat_=13,.lon_=8});
                            result.append({.lat_=14,.lon_=8});
                            result.append({.lat_=14,.lon_=7});
                            result.append({.lat_=16,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=20,.lon_=7});
                            result.append({.lat_=20,.lon_=10});
                            result.append({.lat_=18,.lon_=10});
                            result.append({.lat_=18,.lon_=7});
                            result.append({.lat_=20,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=3,.lon_=7});
                            result.append({.lat_=3,.lon_=10});
                            result.append({.lat_=0,.lon_=10});
                            result.append({.lat_=0,.lon_=7});
                            result.append({.lat_=3,.lon_=7});
                            return result;
                        }()
                    };
                    int counter = 0;
                    for(auto& new_pg:res){
                        if(auto found = std::find_if(etalon.begin(),etalon.end(),
                            [&new_pg](const Polygon& pg){
                            return pg.equivalent(new_pg,false);
                        });found==etalon.end()){
                            std::cout<<"polygon №: "<<counter<<std::endl;
                            print(new_pg);
                            EXPECT_TRUE(false);
                        }
                        else EXPECT_TRUE(true);
                        ++counter;
                    }
                }
                {
                    std::cout<<"test #4"<<std::endl;
                    auto res = pg.cut(line,pg, false);
                    ASSERT_FALSE(res.empty());
                    for(const Polygon& polyg:res)
                        print(polyg);
                    std::vector<Polygon> etalon = {
                        [](){
                            Polygon result;
                            result.append({.lat_=20,.lon_=7});
                            result.append({.lat_=20,.lon_=3});
                            result.append({.lat_=14,.lon_=3});
                            result.append({.lat_=14,.lon_=7});
                            result.append({.lat_=16,.lon_=7});
                            result.append({.lat_=16,.lon_=5});
                            result.append({.lat_=18,.lon_=5});
                            result.append({.lat_=18,.lon_=7});
                            result.append({.lat_=20,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=10.5,.lon_=7});
                            result.append({.lat_=10.5,.lon_=6});
                            result.append({.lat_=8,.lon_=6});
                            result.append({.lat_=8,.lon_=7});
                            result.append({.lat_=9,.lon_=7});
                            result.append({.lat_=9,.lon_=6.5});
                            result.append({.lat_=10,.lon_=6.5});
                            result.append({.lat_=10,.lon_=7});
                            result.append({.lat_=10.5,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=6.75,.lon_=6.75});
                            result.append({.lat_=6.5,.lon_=6.75});
                            result.append({.lat_=6.5,.lon_=7});
                            result.append({.lat_=6.75,.lon_=7});
                            result.append({.lat_=6.75,.lon_=6.75});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=5,.lon_=7});
                            result.append({.lat_=5,.lon_=6});
                            result.append({.lat_=4,.lon_=6});
                            result.append({.lat_=4,.lon_=7});
                            result.append({.lat_=5,.lon_=7});
                            return result;
                        }(),
                        [](){
                            Polygon result;
                            result.append({.lat_=0,.lon_=0});
                            result.append({.lat_=0,.lon_=7});
                            result.append({.lat_=3,.lon_=7});
                            result.append({.lat_=3,.lon_=5});
                            result.append({.lat_=5.5,.lon_=5});
                            result.append({.lat_=5.5,.lon_=7});
                            result.append({.lat_=6,.lon_=7});
                            result.append({.lat_=6,.lon_=6.25});
                            result.append({.lat_=6.25,.lon_=6.25});
                            result.append({.lat_=6.25,.lon_=7});
                            result.append({.lat_=6.375,.lon_=7});
                            result.append({.lat_=6.375,.lon_=6.5});
                            result.append({.lat_=7,.lon_=6.5});
                            result.append({.lat_=7,.lon_=3});
                            result.append({.lat_=8,.lon_=3});
                            result.append({.lat_=12,.lon_=7});
                            result.append({.lat_=13,.lon_=7});
                            result.append({.lat_=13,.lon_=0});
                            result.append({.lat_=0,.lon_=0});
                            return result;
                        }()
                    };
                    int counter = 0;
                    for(auto& new_pg:res){
                        if(auto found = std::find_if(etalon.begin(),etalon.end(),
                            [&new_pg](const Polygon& pg){
                            return pg.equivalent(new_pg,false);
                        });found==etalon.end()){
                            std::cout<<"polygon №: "<<counter<<std::endl;
                            print(new_pg);
                            EXPECT_TRUE(false);
                        }
                        else EXPECT_TRUE(true);
                        ++counter;
                    }
                }
            }
        }
    }
    {
        {
            Line polyline = {
                {.lat_=0, .lon_=0},
                {.lat_=55, .lon_=55}
            };
            Polygon pg;
            pg.append({.lat_=0,.lon_=0});
            pg.append({.lat_=0,.lon_=2});
            pg.append({.lat_=2,.lon_=2});
            pg.append({.lat_=2,.lon_=0});
            pg.append({.lat_=0,.lon_=0});
            std::cout<<"test #5"<<std::endl;
            auto res = pg.cut(polyline,pg, false);
            for(const Polygon& polyg:res)
                print(polyg);
            std::vector<Polygon> etalon = {
                [](){
                    Polygon result;
                    result.append({.lat_=2,.lon_=2});
                    result.append({.lat_=0,.lon_=2});
                    result.append({.lat_=0,.lon_=0});
                    result.append({.lat_=2,.lon_=2});
                    return result;
                }()
            };
            int counter = 0;
            for(auto& new_pg:res){
                if(auto found = std::find_if(etalon.begin(),etalon.end(),
                    [&new_pg](const Polygon& pg){
                    return pg.equivalent(new_pg,false);
                });found==etalon.end()){
                    std::cout<<"polygon №: "<<counter<<std::endl;
                    print(new_pg);
                    EXPECT_TRUE(false);
                }
                else EXPECT_TRUE(true);
                ++counter;
            }
        }
    }
}

TEST(polygon,is_valid_test){
    {
        Polygon pg;
        pg.append({.lat_=0, .lon_=0});  // A
        pg.append({.lat_=0, .lon_=2});  // B
        pg.append({.lat_=2, .lon_=0});  // C
        pg.append({.lat_=2, .lon_=2});  // D
        pg.append({.lat_=0, .lon_=0});  // A — close сработал, ring замкнут
        pg.append({.lat_=0, .lon_=0});  // A — close сработал ещё раз, получили [A,B,C,D,A,A]
        ASSERT_TRUE(!pg.is_valid());     // ← упадёт
    }
    {
        Polygon pg;
        pg.append({.lat_=0, .lon_=0});  // A
        pg.append({.lat_=0, .lon_=2});  // B
        pg.append({.lat_=2, .lon_=2});  // C
        pg.append({.lat_=2, .lon_=2});  // C
        pg.append({.lat_=2, .lon_=0});  // D
        pg.append({.lat_=1, .lon_=0});  // E
        pg.append({.lat_=0.5, .lon_=0});  // E
        pg.append({.lat_=0, .lon_=0});  // A — close сработал, ring замкнут
        pg.append({.lat_=0, .lon_=0});  // A — close сработал ещё раз, получили [A,B,C,D,A,A]
        ASSERT_TRUE(pg.is_valid());     // ← упадёт
    }
    {
        Polygon pg;
        pg.append({.lat_=0, .lon_=0});  // A
        pg.append({.lat_=0, .lon_=2});  // B
        pg.append({.lat_=2, .lon_=0});  // C
        pg.append({.lat_=2, .lon_=2});  // C
        pg.append({.lat_=0, .lon_=0});  // A
        ASSERT_TRUE(!pg.is_valid());
    }
}

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
