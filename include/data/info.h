#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>
#include <cstring>
#include <netinet/in.h>
#include "grib/index_data_info.h"
#include "sublimed_info.h"
#include "serialization.h"
#include "def.h"
#include "types/time_interval.h"
#include "common_data_properties.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "msg.h"
#include "sys/error_print.h"
#include "sys/error_code.h"
#include "data/def.h"
using namespace std::string_literals;

template<Data_t TYPE,Data_f FORMAT>
class SublimedFormatDataInfo;

using SublimedGribDataInfo = SublimedFormatDataInfo<Data_t::METEO,Data_f::GRIB>;

#include <string_view>
#include "definitions/path_process.h"
namespace fs = std::filesystem;

template<Data_t TYPE,Data_f FORMAT>
class ProxyDataInfo;

using GribProxyDataInfo = ProxyDataInfo<Data_t::METEO,Data_f::GRIB>;

template<>
class ProxyDataInfo<Data_t::METEO,Data_f::GRIB>{
    public:
    using data_t = std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<Grib1CommonDataProperties>,std::vector<IndexDataInfo<API::GRIB1>>>>;
    using sublimed_data_t = std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<Grib1CommonDataProperties>,std::deque<GribSublimedDataInfoStruct>>>;
    protected:
    data_t info_;
    API::ErrorData::Code<API::GRIB1>::value err = API::ErrorData::Code<API::GRIB1>::NONE_ERR;
    friend class Index;
    friend class Integrity;
    friend class Extract;
    public:
    ProxyDataInfo() =default;
    ProxyDataInfo(const ProxyDataInfo& other):info_(other.info_){}
    ProxyDataInfo(ProxyDataInfo&& other):info_(std::move(other.info_)){}
    ProxyDataInfo(const data_t& info):
    info_(info){}
    ProxyDataInfo(data_t&& info):
    info_(std::move(info)){}
    void add_info(const path::Storage<false>& path,Grib1CommonDataProperties&& cmn,std::ranges::random_access_range auto&& index_info){
            info_[path][std::make_shared<Grib1CommonDataProperties>(std::move(cmn))].append_range(std::forward<decltype(index_info)>(index_info));
    }
    void add_info(const path::Storage<false>& path,Grib1CommonDataProperties&& cmn,IndexDataInfo<API::GRIB1>&& index_info){
        info_[path][std::make_shared<Grib1CommonDataProperties>(std::move(cmn))].push_back(std::move(index_info));
    }
    void add_info(const path::Storage<false>& path,Grib1CommonDataProperties&& cmn,const IndexDataInfo<API::GRIB1>& index_info){
        info_[path][std::make_shared<Grib1CommonDataProperties>(std::move(cmn))].push_back(index_info);
    }
    void add_info(const path::Storage<false>& path, const GribMsgDataInfo& msg_info) noexcept;
    void add_info(const path::Storage<false>& path, GribMsgDataInfo&& msg_info) noexcept;
    API::ErrorData::Code<API::GRIB1>::value error() const;
    const data_t& data() const;
    void swap(ProxyDataInfo& other) noexcept;
    //void read(const fs::path& path);
    //void write(const fs::path& dir);
    SublimedGribDataInfo sublime();
};
#include <unordered_set>
#include "definitions/path_process.h"
#include "gtest/gtest_prod.h"
template<>
class SublimedFormatDataInfo<Data_t::METEO,Data_f::GRIB>
{
    public:
    using sublimed_data_t = std::unordered_map<path::Storage<false>,
    std::map<std::shared_ptr<Grib1CommonDataProperties>,std::deque<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>;
    template<bool,auto>
    friend struct serialization::Serialize;
    template<bool,auto>
    friend struct serialization::Deserialize;
    template<auto>
    friend struct serialization::Serial_size;
    template<auto>
    friend struct serialization::Min_serial_size;
    template<auto>
    friend struct serialization::Max_serial_size;
    FRIEND_TEST(Serialization,SublimedFormatDataInfo);
    private:
    sublimed_data_t info_;
    std::unordered_set<path::Storage<true>> paths_;
    public:
    SublimedFormatDataInfo() = default;
    SublimedFormatDataInfo(SublimedFormatDataInfo&& other):
    info_(std::move(other.info_)){}
    SublimedFormatDataInfo(const SublimedFormatDataInfo&) = delete;
    const sublimed_data_t& data() const{
        return info_;
    }

    const std::unordered_set<path::Storage<true>>& paths() const{
        return paths_;
    }
    void add_data(const SublimedFormatDataInfo& grib_data){
        add_data(grib_data.info_);
    }
    void add_data(const SublimedFormatDataInfo::sublimed_data_t& grib_data){
        std::unordered_set<path::Storage<true>>::iterator iter;
        for(auto& [path,file_data]:grib_data){
            if(!path.path_.empty()){
                auto found = info_.find(path);
                if(found==info_.end()){
                    found = info_.insert({path,file_data}).first;
                    iter = paths_.insert(found->first).first;
                    assert(paths_.contains(found->first));
                }
                else{
                    assert(paths_.contains(found->first));
                    info_[found->first].insert(file_data.begin(),file_data.end());
                }
            }
        }
    }
    void add_data(SublimedFormatDataInfo&& grib_data){
        add_data(std::move(grib_data.info_));
    }
    void add_data(SublimedFormatDataInfo::sublimed_data_t&& grib_data){
        for(auto&& [path,file_data]:grib_data){
            if(!path.path_.empty()){
                auto found = info_.find(path);
                if(found==info_.end()){
                    found = info_.insert({std::move(path),std::move(file_data)}).first;
                    paths_.insert(found->first);
                    assert(paths_.contains(found->first));
                }
                else{
                    assert(paths_.contains(found->first));
                    info_[found->first].insert( std::make_move_iterator(file_data.begin()),
                                                std::make_move_iterator(file_data.end()));                    
                }
            }
        }        
    }
    void add_data(GribProxyDataInfo& grib_data){
        add_data(grib_data.sublime());
    }
};

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            SerializationEC err = SerializationEC::NONE;
            using val_t = type::sublimed_data_t::value_type;
            auto existing_paths = std::ranges::copy_if_result(msg.info_,[](const val_t& pair) noexcept{
                                        const auto& pathstore = std::get<0>(pair);
                                        if(fs::exists(pathstore.path_) || 
                                        pathstore.type_==path::TYPE::HOST){
                                            return true;
                                        }
                                        else return false;
                                    }).in;
            err = serialize<NETWORK_ORDER>(existing_paths,buf);
            return err;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            SerializationEC err=SerializationEC::NONE;
            err = deserialize<NETWORK_ORDER>(msg.info_,buf);
            for(auto& [path,val]:msg.info_)
                msg.paths_.insert(path);
            return err;
        }
    };

    template<>
    struct Serial_size<SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        size_t operator()(const type& msg) const noexcept{
            using val_t = type::sublimed_data_t::value_type;
            auto existing_paths = std::ranges::copy_if_result(msg.info_,[](const val_t& pair) noexcept{
                                        const auto& pathstore = std::get<0>(pair);
                                        if(fs::exists(pathstore.path_) || 
                                        pathstore.type_==path::TYPE::HOST){
                                            return true;
                                        }
                                        else return false;
                                    }).in;
            return serial_size(existing_paths);
        }
    };

    template<>
    struct Min_serial_size<SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::info_)>();
        }();
    };

    template<>
    struct Max_serial_size<SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::info_)>();
        }();
    };
}

//@todo make possible serialization
static_assert(requires{requires std::ranges::range<GribProxyDataInfo::sublimed_data_t>;});

static_assert(serialization::serialize_concept<true,std::pair<std::shared_ptr<Grib1CommonDataProperties> const, std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>, std::allocator<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>>);
static_assert(serialization::serialize_concept<false,std::pair<std::shared_ptr<Grib1CommonDataProperties> const, std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>, std::allocator<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>>);
static_assert(requires {requires serialization::serialize_concept<true,std::optional<GribProxyDataInfo::sublimed_data_t>>;});
static_assert(requires {requires serialization::serialize_concept<false,std::optional<GribProxyDataInfo::sublimed_data_t>>;});
static_assert(serialization::deserialize_concept<true,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>);
static_assert(serialization::deserialize_concept<false,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>);
static_assert(serialization::serialize_concept<true,std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<Grib1CommonDataProperties>,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>>);
static_assert(serialization::serialize_concept<false,std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<Grib1CommonDataProperties>,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>>);
//std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>