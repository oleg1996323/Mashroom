#include "serialization.h"
#include <gtest/gtest.h>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <utility>

class SerializableItems:public testing::Test{
    protected:
    SerializableItems(){
        double fp_val_1 = std::rand()/std::rand();
        double fp_val_2 = std::rand()/std::rand();
        integer_1=std::rand();
        integer_2=std::rand();
        
        tp_ = std::chrono::system_clock::now();
        dur_ = std::chrono::duration_cast<std::chrono::hours>(std::chrono::seconds(std::rand()));
        fp_unique_ = std::make_unique<double>(std::rand()/std::rand());
        fp_shared_ = std::make_shared<double>(std::rand()/std::rand());
        pair_ = std::make_pair(std::rand()/std::rand(),std::rand());
        for(int i=0;i<100;++i)
            vector_pairs_.push_back(std::make_pair(std::rand()/std::rand(),std::make_shared<int>(std::rand())));
        for(int i=0;i<50;++i)
            list_pairs_.push_back(std::make_pair(std::rand(),std::rand()));
        for(int i=0;i<30;++i)
            deque_tp_.push_back(std::chrono::system_clock::now());
        for(int i=0;i<65;++i)
            set_int_.insert(std::rand());
        for(int i=0;i<65;++i)
            uset_int_.insert(std::rand());
        for(int i=0;i<65;++i)
            map_int_.emplace(std::rand(),std::rand());
        for(int i=0;i<65;++i)
            umap_int_.emplace(std::rand(),std::rand());
    }
    ~SerializableItems() = default;
    double fp_val_1;
    double fp_val_2;
    int integer_1;
    int integer_2;
    std::chrono::system_clock::time_point tp_;
    std::chrono::system_clock::duration dur_;
    std::unique_ptr<double> fp_unique_;
    std::shared_ptr<double> fp_shared_;
    std::pair<double,int> pair_;
    std::vector<std::pair<double,std::shared_ptr<int>>> vector_pairs_;
    std::list<std::pair<int,int>> list_pairs_;
    std::deque<std::chrono::system_clock::time_point> deque_tp_;
    std::set<int> set_int_;
    std::unordered_set<int> uset_int_;
    std::map<int,int> map_int_;
    std::unordered_map<int,int> umap_int_;
};

TEST_F(SerializableItems,SimpleSerialization){
    using namespace serialization;
    std::vector<char> buf;
    serialize<true>(fp_val_1,buf);
    serialize<true>(fp_val_1,buf);
}