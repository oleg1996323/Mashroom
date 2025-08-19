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
#include "../../utility/random_char.h"

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

struct CheckValues{
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

TEST(Serialization, SerializeInt){
    using namespace serialization;
    std::vector<char> buf;
    int integer_1 = std::rand();
    if(integer_1>0)
        integer_1=-integer_1;
    int integer_2 = std::rand();
    ASSERT_EQ(serialization::min_serial_size(int()),sizeof(int));
    ASSERT_EQ(serialization::max_serial_size(int()),sizeof(int));
    ASSERT_EQ(serialize<true>(integer_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(integer_2,buf),serialization::SerializationEC::NONE);

    int reversed_1=std::byteswap(integer_1);
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(int))==0);
    int reversed_2=std::byteswap(integer_2);
    EXPECT_TRUE(std::memcmp(&reversed_2,buf.data()+sizeof(reversed_1),sizeof(int))==0);

    int check_int_1 = 0;
    int check_int_2 = 0;
    ASSERT_EQ(deserialize<true>(check_int_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(check_int_2,std::span(buf).subspan(serial_size(check_int_1))),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_int_1,integer_1);
    EXPECT_EQ(check_int_2,integer_2);
}

TEST(Serialization, SerializeFloatingPoint){
    using namespace serialization;
    std::vector<char> buf;
    double fp_1 = std::rand()/std::rand();
    double fp_2 = std::rand();
    ASSERT_EQ(serialization::min_serial_size(double()),sizeof(double));
    ASSERT_EQ(serialization::max_serial_size(double()),sizeof(double));
    ASSERT_EQ(serialize<true>(fp_1,buf),serialization::SerializationEC::NONE);
    ASSERT_EQ(serialize<true>(fp_2,buf),serialization::SerializationEC::NONE);

    auto reversed_1=std::byteswap(to_integer(fp_1));
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(double))==0);
    auto reversed_2=std::byteswap(to_integer(fp_2));
    EXPECT_TRUE(std::memcmp(&reversed_2,buf.data()+sizeof(reversed_1),sizeof(double))==0);

    double check_fp_1 = 0;
    double check_fp_2 = 0;
    ASSERT_EQ(deserialize<true>(check_fp_1,std::span(buf)),serialization::SerializationEC::NONE);
    ASSERT_EQ(deserialize<true>(check_fp_2,std::span(buf).subspan(serial_size(check_fp_1))),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_fp_1,fp_1);
    EXPECT_EQ(check_fp_2,fp_2);
}

TEST(Serialization, SerializeTimePoint){
    using namespace serialization;
    std::vector<char> buf;
    std::chrono::system_clock::time_point utc_tp = std::chrono::system_clock::now();
    ASSERT_EQ(serialization::min_serial_size(utc_tp),sizeof(utc_tp));
    ASSERT_EQ(serialization::max_serial_size(utc_tp),sizeof(utc_tp));
    ASSERT_EQ(serialize<true>(utc_tp,buf),serialization::SerializationEC::NONE);

    auto reversed_1=std::byteswap(utc_tp.time_since_epoch().count());
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(reversed_1))==0);

    std::chrono::system_clock::time_point check_tp{};
    ASSERT_EQ(deserialize<true>(check_tp,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_tp,utc_tp);
}

TEST(Serialization, SerializeDuration){
    using namespace serialization;
    std::vector<char> buf;
    std::chrono::system_clock::duration utc_tp = (std::chrono::system_clock::now()-std::chrono::years(20)-std::chrono::months(50)).time_since_epoch();
    ASSERT_EQ(serialization::min_serial_size(utc_tp),sizeof(utc_tp));
    ASSERT_EQ(serialization::max_serial_size(utc_tp),sizeof(utc_tp));
    ASSERT_EQ(serialize<true>(utc_tp,buf),serialization::SerializationEC::NONE);

    auto reversed_1=std::byteswap(utc_tp.count());
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(reversed_1))==0);

    std::chrono::system_clock::duration check_tp{};
    ASSERT_EQ(deserialize<true>(check_tp,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_tp,utc_tp);
}

TEST(Serialization, SerializeUniquePtr){
    using namespace serialization;
    std::vector<char> buf;
    std::unique_ptr<double> fp_unique_ = std::make_unique<double>(std::rand()/std::rand());
    ASSERT_EQ(serialization::min_serial_size(fp_unique_),sizeof(bool));
    ASSERT_EQ(serialization::max_serial_size(fp_unique_),sizeof(bool)+sizeof(double));
    ASSERT_EQ(serialize<true>(fp_unique_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(fp_unique_));
    auto reversed_1=std::byteswap(to_integer(*fp_unique_.get()));
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data()+min_serial_size(fp_unique_),sizeof(reversed_1))==0);

    std::unique_ptr<double> check_tp{};
    ASSERT_EQ(deserialize<true>(check_tp,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(*check_tp.get(),*fp_unique_.get());

    fp_unique_.reset();
    buf.clear();
    ASSERT_EQ(serialize<true>(fp_unique_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(fp_unique_));

    check_tp.reset();
    ASSERT_EQ(deserialize<true>(check_tp,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_tp,fp_unique_);
    EXPECT_FALSE(check_tp);
}

TEST(Serialization, SerializeSharedPtr){
    using namespace serialization;
    std::vector<char> buf;
    std::shared_ptr<double> fp_shared_ = std::make_shared<double>(std::rand()/std::rand());
    ASSERT_EQ(serialization::min_serial_size(fp_shared_),sizeof(bool));
    ASSERT_EQ(serialization::max_serial_size(fp_shared_),sizeof(bool)+sizeof(double));
    ASSERT_EQ(serialize<true>(fp_shared_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(fp_shared_));
    auto reversed_1=std::byteswap(to_integer(*fp_shared_.get()));
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data()+min_serial_size(fp_shared_),sizeof(reversed_1))==0);

    std::shared_ptr<double> check_fp_shared{};
    ASSERT_EQ(deserialize<true>(check_fp_shared,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(*check_fp_shared.get(),*fp_shared_.get());

    fp_shared_.reset();
    buf.clear();
    ASSERT_EQ(serialize<true>(fp_shared_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(fp_shared_));

    check_fp_shared.reset();
    ASSERT_EQ(deserialize<true>(check_fp_shared,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_fp_shared,fp_shared_);
    EXPECT_FALSE(check_fp_shared);
}

TEST(Serialization, SerializePair){
    using namespace serialization;
    std::vector<char> buf;
    std::pair<double,int> pair_ = std::make_pair(std::rand()/std::rand(),std::rand());
    ASSERT_EQ(serialization::min_serial_size(pair_),min_serial_size(pair_.first)+min_serial_size(pair_.second));
    ASSERT_EQ(serialization::max_serial_size(pair_),max_serial_size(pair_.first)+max_serial_size(pair_.second));
    ASSERT_EQ(serialize<true>(pair_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(pair_));
    auto reversed_1=std::make_pair(std::byteswap(to_integer(pair_.first)),std::byteswap(pair_.second));
    EXPECT_TRUE(std::memcmp(&reversed_1,buf.data(),sizeof(reversed_1))==0);

    std::pair<double,int> check_pair{};
    ASSERT_EQ(deserialize<true>(check_pair,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_pair,pair_);
}

TEST(Serialization, SerializeOptional){
    using namespace serialization;
    std::vector<char> buf;
    std::optional<double> opt_ = std::make_optional(std::rand()/std::rand());
    ASSERT_EQ(serialization::min_serial_size(opt_),sizeof(bool));
    ASSERT_EQ(serialization::max_serial_size(opt_),sizeof(bool)+sizeof(opt_.value()));
    ASSERT_EQ(serialize<true>(opt_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(opt_));
    auto reversed_1=std::make_optional(std::byteswap(to_integer(opt_.value())));
    EXPECT_TRUE(std::memcmp(&reversed_1.value(),buf.data()+min_serial_size(reversed_1),sizeof(reversed_1.value()))==0);

    std::optional<double> check_opt{};
    ASSERT_EQ(deserialize<true>(check_opt,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_opt,opt_);

    opt_.reset();
    buf.clear();
    ASSERT_EQ(serialize<true>(opt_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(opt_));

    check_opt.reset();
    ASSERT_EQ(deserialize<true>(check_opt,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_opt,opt_);
    EXPECT_FALSE(check_opt);
}

TEST(Serialization, SerializeList){
    using namespace serialization;
    std::vector<char> buf;
    std::list<std::pair<int,int>> list_;
    for(int i=0;i<50;++i)
        list_.push_back(std::make_pair(std::rand(),std::rand()));
    ASSERT_EQ(serialization::min_serial_size(list_),sizeof(size_t));
    ASSERT_EQ(serialization::max_serial_size(list_),std::numeric_limits<size_t>::max());
    ASSERT_EQ(serialize<true>(list_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(list_));
    std::vector<std::pair<int,int>> reversed_1;
    reversed_1.reserve(list_.size());
    for(auto& [f,s]:list_)
        reversed_1.push_back(std::make_pair(std::byteswap(f),std::byteswap(s)));
    EXPECT_EQ(std::memcmp(reversed_1.data(),buf.data()+serial_size(reversed_1.size()),sizeof(reversed_1.size())),0);
    EXPECT_EQ(serial_size(reversed_1),buf.size());

    std::list<std::pair<int,int>> check_list{};
    ASSERT_EQ(deserialize<true>(check_list,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_list,list_);
}

TEST(Serialization, SerializeVector){
    using namespace serialization;
    std::vector<char> buf;
    std::vector<std::pair<int,int>> vector_;
    for(int i=0;i<50;++i)
        vector_.push_back(std::make_pair(std::rand(),std::rand()));
    
    ASSERT_EQ(serialization::min_serial_size(vector_),sizeof(size_t));
    ASSERT_EQ(serialization::max_serial_size(vector_),std::numeric_limits<size_t>::max());
    ASSERT_EQ(serialize<true>(vector_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(vector_));
    std::vector<std::pair<int,int>> reversed_1;
    reversed_1.reserve(vector_.size());
    for(auto& [f,s]:vector_)
        reversed_1.push_back(std::make_pair(std::byteswap(f),std::byteswap(s)));
    EXPECT_EQ(std::memcmp(reversed_1.data(),buf.data()+serial_size(reversed_1.size()),sizeof(reversed_1.size())),0);
    EXPECT_EQ(serial_size(reversed_1),buf.size());

    std::vector<std::pair<int,int>> check_vector{};
    ASSERT_EQ(deserialize<true>(check_vector,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_vector,vector_);
}

TEST(Serialization, SerializeString){
    using namespace serialization;
    std::vector<char> buf;
    std::string string_;
    string_.resize(10);
    std::generate(string_.begin(),string_.end(),getRandomChar);
    
    ASSERT_EQ(serialization::min_serial_size(string_),sizeof(size_t));
    ASSERT_EQ(serialization::max_serial_size(string_),std::numeric_limits<size_t>::max());
    ASSERT_EQ(serialize<true>(string_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(string_));

    std::string check_string;
    ASSERT_EQ(deserialize<true>(check_string,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_string,string_);
}

TEST(Serialization, SerializeDeque){
    using namespace serialization;
    std::vector<char> buf;
    std::deque<std::pair<int,int>> deque_;
    for(int i=0;i<50;++i)
        deque_.push_back(std::make_pair(std::rand(),std::rand()));
    ASSERT_EQ(serialization::min_serial_size(deque_),sizeof(size_t));
    ASSERT_EQ(serialization::max_serial_size(deque_),std::numeric_limits<size_t>::max());
    ASSERT_EQ(serialize<true>(deque_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(deque_));
    std::vector<std::pair<int,int>> reversed_1;
    reversed_1.reserve(deque_.size());
    for(auto& [f,s]:deque_)
        reversed_1.push_back(std::make_pair(std::byteswap(f),std::byteswap(s)));
    EXPECT_EQ(std::memcmp(reversed_1.data(),buf.data()+serial_size(reversed_1.size()),sizeof(reversed_1.size())),0);
    EXPECT_EQ(serial_size(reversed_1),buf.size());

    std::deque<std::pair<int,int>> check_deque{};
    ASSERT_EQ(deserialize<true>(check_deque,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_deque,deque_);
}

TEST(Serialization, SerializeSet){
    using namespace serialization;
    std::vector<char> buf;
    std::set<double> set_;
    for(int i=0;i<50;++i){
        auto inserted = double(std::rand())/std::rand();
        set_.insert(inserted);
    }
    ASSERT_EQ(serialization::min_serial_size(set_),sizeof(size_t));
    ASSERT_EQ(serialization::max_serial_size(set_),std::numeric_limits<size_t>::max());
    ASSERT_EQ(serialize<true>(set_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(set_));
    std::vector<double> reversed_1;
    reversed_1.reserve(set_.size());
    for(auto& val:set_){
        //if not use to_float the double will contains only the whole part
        reversed_1.push_back(to_float(std::byteswap(to_integer(val))));
    }
    EXPECT_EQ(std::memcmp(reversed_1.data(),buf.data()+serial_size(reversed_1.size()),sizeof(reversed_1.size())),0);
    
    EXPECT_EQ(serial_size(reversed_1),buf.size());

    std::set<double> check_set{};
    ASSERT_EQ(deserialize<true>(check_set,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_set,set_);
}

TEST(Serialization, SerializeUnorderedSet){
    using namespace serialization;
    std::vector<char> buf;
    std::unordered_set<double> uset_;
    for(int i=0;i<50;++i){
        auto inserted = double(std::rand())/std::rand();
        uset_.insert(inserted);
    }
    ASSERT_EQ(serialization::min_serial_size(uset_),sizeof(size_t));
    ASSERT_EQ(serialization::max_serial_size(uset_),std::numeric_limits<size_t>::max());
    ASSERT_EQ(serialize<true>(uset_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(uset_));
    std::vector<double> reversed_1;
    reversed_1.reserve(uset_.size());
    for(auto& val:uset_){
        //if not use to_float the double will contains only the whole part
        reversed_1.push_back(to_float(std::byteswap(to_integer(val))));
    }
    EXPECT_EQ(std::memcmp(reversed_1.data(),buf.data()+serial_size(reversed_1.size()),sizeof(reversed_1.size())),0);
    
    EXPECT_EQ(serial_size(reversed_1),buf.size());

    std::unordered_set<double> check_uset{};
    ASSERT_EQ(deserialize<true>(check_uset,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_uset,uset_);
}

TEST(Serialization, SerializeMap){
    using namespace serialization;
    std::vector<char> buf;
    std::map<double,double> map_;
    for(int i=0;i<50;++i){
        auto key = double(std::rand())/std::rand();
        auto val = double(std::rand())/std::rand();
        map_.emplace(key,val);
    }
    ASSERT_EQ(serialization::min_serial_size(map_),sizeof(size_t));
    ASSERT_EQ(serialization::max_serial_size(map_),std::numeric_limits<size_t>::max());
    ASSERT_EQ(serialize<true>(map_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(map_));
    std::vector<decltype(map_)::value_type> reversed_1;
    reversed_1.reserve(map_.size());
    for(auto& [key,val]:map_){
        //if not use to_float the double will contains only the 
        reversed_1.push_back(std::make_pair(to_float(std::byteswap(to_integer(key))),to_float(std::byteswap(to_integer(val)))));
    }
    EXPECT_EQ(std::memcmp(reversed_1.data(),buf.data()+serial_size(reversed_1.size()),sizeof(reversed_1.size())),0);
    
    EXPECT_EQ(serial_size(reversed_1),buf.size());

    std::map<double,double> check_map{};
    ASSERT_EQ(deserialize<true>(check_map,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_map,map_);
}

TEST(Serialization, SerializeUMap){
    using namespace serialization;
    std::vector<char> buf;
    std::unordered_map<double,double> umap_;
    for(int i=0;i<5;++i){
        auto key = double(std::rand())/std::rand();
        auto val = double(std::rand())/std::rand();
        umap_.emplace(key,val);
    }
    ASSERT_EQ(serialization::min_serial_size(umap_),sizeof(size_t));
    ASSERT_EQ(serialization::max_serial_size(umap_),std::numeric_limits<size_t>::max());
    ASSERT_EQ(serialize<true>(umap_,buf),serialization::SerializationEC::NONE);
    EXPECT_EQ(buf.size(),serial_size(umap_));
    std::vector<decltype(umap_)::value_type> reversed_1;
    reversed_1.reserve(umap_.size());
    for(auto& [key,val]:umap_)
        //if not use to_float the double will contains only the whole part
        reversed_1.push_back(std::make_pair(to_float(std::byteswap(to_integer(key))),to_float(std::byteswap(to_integer(val)))));
    EXPECT_EQ(std::memcmp(reversed_1.data(),buf.data()+serial_size(reversed_1.size()),sizeof(reversed_1.size())),0);
    
    EXPECT_EQ(serial_size(reversed_1),buf.size());

    std::unordered_map<double,double> check_umap{};
    ASSERT_EQ(deserialize<true>(check_umap,std::span(buf)),serialization::SerializationEC::NONE);
    EXPECT_EQ(check_umap,umap_);
}

TEST(Serialization,Variant){
    using namespace serialization;
    std::vector<char> buf;
    std::vector<char> reversed_1;
    std::variant<std::monostate,int,double,std::pair<std::string,std::string>> var_1;
    {
        EXPECT_EQ(serial_size(var_1),sizeof(size_t)+serial_size(std::variant_alternative_t<0,decltype(var_1)>()));
        ASSERT_EQ(serialize<true>(var_1,buf),serialization::SerializationEC::NONE);
        size_t index = std::byteswap(var_1.index());
        reversed_1.insert(reversed_1.cend(),reinterpret_cast<const char*>(&index),reinterpret_cast<const char*>(&index)+sizeof(size_t));
        EXPECT_EQ(reversed_1,buf);
        decltype(var_1) var_tmp;
        EXPECT_EQ(deserialize<true>(var_tmp,std::span<const char>(buf)),SerializationEC::NONE);
        EXPECT_EQ(var_tmp,var_1);
        reversed_1.clear();
        buf.clear();
    }
    {
        var_1=2;
        EXPECT_EQ(serial_size(var_1),sizeof(size_t)+serial_size(std::variant_alternative_t<1,decltype(var_1)>()));
        ASSERT_EQ(serialize<true>(var_1,buf),serialization::SerializationEC::NONE);
        size_t index = std::byteswap(var_1.index());
        reversed_1.insert(reversed_1.cend(),reinterpret_cast<const char*>(&index),reinterpret_cast<const char*>(&index)+sizeof(size_t));
        auto tmp = std::byteswap(std::get<1>(var_1));
        reversed_1.insert(reversed_1.cend(),reinterpret_cast<const char*>(&tmp),reinterpret_cast<const char*>(&tmp)+sizeof(tmp));
        EXPECT_EQ(reversed_1,buf);
        decltype(var_1) var_tmp;
        EXPECT_EQ(deserialize<true>(var_tmp,std::span<const char>(buf)),SerializationEC::NONE);
        EXPECT_EQ(var_tmp,var_1);
        reversed_1.clear();
        buf.clear();
    }
    {
        var_1=2.2;
        EXPECT_EQ(serial_size(var_1),sizeof(size_t)+serial_size(std::variant_alternative_t<2,decltype(var_1)>()));
        ASSERT_EQ(serialize<true>(var_1,buf),serialization::SerializationEC::NONE);
        size_t index = std::byteswap(var_1.index());
        reversed_1.insert(reversed_1.cend(),reinterpret_cast<const char*>(&index),reinterpret_cast<const char*>(&index)+sizeof(size_t));
        auto tmp = std::byteswap(to_integer(std::get<2>(var_1)));
        reversed_1.insert(reversed_1.cend(),reinterpret_cast<const char*>(&tmp),reinterpret_cast<const char*>(&tmp)+sizeof(tmp));
        EXPECT_EQ(reversed_1,buf);
        decltype(var_1) var_tmp;
        EXPECT_EQ(deserialize<true>(var_tmp,std::span<const char>(buf)),SerializationEC::NONE);
        EXPECT_EQ(var_tmp,var_1);
        reversed_1.clear();
        buf.clear();
    }
    {
        var_1=std::make_pair<std::string,std::string>("string1","string2");
        EXPECT_EQ(serial_size(var_1),sizeof(size_t)+serial_size(std::pair<std::string,std::string>("string1","string2")));
        ASSERT_EQ(serialize<true>(var_1,buf),serialization::SerializationEC::NONE);
        size_t index = std::byteswap(var_1.index());
        reversed_1.insert(reversed_1.cend(),reinterpret_cast<const char*>(&index),reinterpret_cast<const char*>(&index)+sizeof(size_t));
        auto tmp = std::get<3>(var_1);
        EXPECT_EQ(serialize<true>(tmp,reversed_1),SerializationEC::NONE);
        EXPECT_EQ(reversed_1,buf);
        decltype(var_1) var_tmp;
        EXPECT_EQ(deserialize<true>(var_tmp,std::span<const char>(buf)),SerializationEC::NONE);
        EXPECT_EQ(var_tmp,var_1);
        reversed_1.clear();
        buf.clear();
    }
}

TEST(Serialization, SerialLimits){
    using namespace serialization;
    ASSERT_EQ(min_serial_size(std::chrono::system_clock::time_point()),sizeof(std::chrono::system_clock::time_point));
    ASSERT_EQ(max_serial_size(std::chrono::system_clock::time_point()),sizeof(std::chrono::system_clock::time_point));
    ASSERT_EQ(min_serial_size(std::chrono::system_clock::duration()),sizeof(std::chrono::system_clock::duration));
    ASSERT_EQ(max_serial_size(std::chrono::system_clock::duration()),sizeof(std::chrono::system_clock::duration));
    ASSERT_EQ(min_serial_size(std::shared_ptr<int>{}),sizeof(bool));
    ASSERT_EQ(max_serial_size(std::shared_ptr<int>{}),sizeof(bool)+sizeof(int));
    ASSERT_EQ(min_serial_size(std::chrono::years()),sizeof(std::chrono::system_clock::duration));
    ASSERT_EQ(max_serial_size(std::chrono::system_clock::duration()),sizeof(std::chrono::system_clock::duration));
    ASSERT_EQ(min_serial_size(std::vector<ptrdiff_t>{}),sizeof(size_t));
    ASSERT_EQ(max_serial_size(std::vector<ptrdiff_t>{}),std::numeric_limits<size_t>::max());
    ASSERT_EQ(min_serial_size(std::variant<std::monostate,int,double,std::pair<std::string,std::string>>{}),sizeof(size_t));
    ASSERT_EQ(max_serial_size(std::variant<std::monostate,int,double,std::pair<std::string,std::string>>{}),std::numeric_limits<size_t>::max());
}

int main(int argc, char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}