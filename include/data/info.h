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
class DataInfo;

using GribDataInfo = DataInfo<Data_t::METEO,Data_f::GRIB>;

template<>
class DataInfo<Data_t::METEO,Data_f::GRIB>{
    public:
    using data_t = std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<Grib1CommonDataProperties>,std::vector<IndexDataInfo<API::GRIB1>>>>;
    using sublimed_data_t = std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<Grib1CommonDataProperties>,std::vector<GribSublimedDataInfoStruct>>>;
    protected:
    data_t info_;
    API::ErrorData::Code<API::GRIB1>::value err = API::ErrorData::Code<API::GRIB1>::NONE_ERR;
    friend class Index;
    friend class Integrity;
    friend class Extract;
    public:
    DataInfo() =default;
    DataInfo(const DataInfo& other):info_(other.info_){}
    DataInfo(DataInfo&& other):info_(std::move(other.info_)){}
    DataInfo(const data_t& info):
    info_(info){}
    DataInfo(data_t&& info):
    info_(std::move(info)){}
    template<typename GCDI = IndexDataInfo<API::GRIB1>>
    void add_info(const path::Storage<false>& path,Grib1CommonDataProperties&& cmn,GCDI&& index_info){
        if constexpr(std::is_same_v<std::vector<IndexDataInfo<API::GRIB1>>,std::decay_t<GCDI>>){
            info_[path][std::make_shared<Grib1CommonDataProperties>(std::move(cmn))].append_range(std::forward<GCDI>(index_info));
        }
        else
            info_[path][std::make_shared<Grib1CommonDataProperties>(std::move(cmn))].push_back(std::forward<GCDI>(index_info));
    }
    void add_info(const path::Storage<false>& path, const GribMsgDataInfo& msg_info) noexcept;
    void add_info(const path::Storage<false>& path, GribMsgDataInfo&& msg_info) noexcept;
    API::ErrorData::Code<API::GRIB1>::value error() const;
    const data_t& data() const;
    void swap(DataInfo& other) noexcept;
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
    std::map<std::shared_ptr<Grib1CommonDataProperties>,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>;
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
    void add_data(SublimedFormatDataInfo& grib_data){
        for(auto& [path,file_data]:grib_data.info_){
            auto found = info_.find(path);
            if(found==info_.end())
                info_[path].swap(file_data);
            else 
                info_[found->first].swap(file_data);
        }
    }
    void add_data(SublimedFormatDataInfo::sublimed_data_t& grib_data){
        for(auto& [path,file_data]:grib_data){
            if(!path.path_.empty()){
                auto found = info_.find(path);
                if(found==info_.end())
                    info_[path].swap(file_data);
                else 
                    info_[found->first].swap(file_data);
                paths_.insert(path);
            }
        }
    }
    void add_data(SublimedFormatDataInfo&& grib_data){
        for(auto& [path,file_data]:grib_data.info_){
            if(!path.path_.empty()){
                auto found = info_.find(path);
                if(found==info_.end())
                    info_[path].swap(file_data);
                else 
                    info_[found->first].swap(file_data);
                paths_.insert(path);
            }
        }
    }
    void add_data(SublimedFormatDataInfo::sublimed_data_t&& grib_data){
        for(auto& [path,file_data]:grib_data){
            if(!path.path_.empty()){
                auto found = info_.find(path);
                if(found==info_.end()){
                    info_[path].swap(file_data);
                }
                else{
                    info_[found->first].swap(file_data);
                }
                paths_.insert(path);
            }
        }        
    }
    void add_data(GribDataInfo& grib_data){
        add_data(grib_data.sublime());
    }
};

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            SerializationEC err = SerializationEC::NONE;
            auto type_enum = __Data__::FORMAT::GRIB;
            using val_t = type::sublimed_data_t::value_type;
            serialize<NETWORK_ORDER>(type_enum,buf);
            auto existing_paths = std::ranges::copy_if_result(msg.info_,[](const val_t& pair) noexcept{
                                        const auto& pathstore = std::get<0>(pair);
                                        if(fs::exists(pathstore.path_) || 
                                        pathstore.type_==path::TYPE::HOST){
                                            return true;
                                        }
                                        else return false;
                                    }).in;
            // size_t info_sz = existing_paths.size();
            err = serialize<NETWORK_ORDER>(existing_paths,buf);
            // for(const auto& [path,filedata]: existing_paths)
            // {
            //     if(err==SerializationEC::NONE)
            //         err = serialize<NETWORK_ORDER>(path,buf);
            //     if(err==SerializationEC::NONE)
            //         err = serialize<NETWORK_ORDER>(filedata,buf);
            // }
            return err;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            SerializationEC err=SerializationEC::NONE;
            __Data__::FORMAT enum_type = __Data__::FORMAT::UNDEF;
            err = deserialize<NETWORK_ORDER>(enum_type,buf);
            if(err==SerializationEC::NONE)
                err = deserialize<NETWORK_ORDER>(msg.info_,buf.subspan(serial_size(enum_type)));
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
            return serial_size(__Data__::FORMAT::GRIB,existing_paths);
        }
    };

    template<>
    struct Min_serial_size<SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(__Data__::FORMAT::GRIB),decltype(type::info_)>();
        }();
    };

    template<>
    struct Max_serial_size<SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(__Data__::FORMAT::GRIB),decltype(type::info_)>();
        }();
    };
}

//@todo make possible serialization
static_assert(requires{requires std::ranges::range<GribDataInfo::sublimed_data_t>;});

static_assert(serialization::serialize_concept<true,std::pair<std::shared_ptr<Grib1CommonDataProperties> const, std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>, std::allocator<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>>);
static_assert(serialization::serialize_concept<false,std::pair<std::shared_ptr<Grib1CommonDataProperties> const, std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>, std::allocator<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>>);
static_assert(requires {requires serialization::serialize_concept<true,std::optional<GribDataInfo::sublimed_data_t>>;});
static_assert(requires {requires serialization::serialize_concept<false,std::optional<GribDataInfo::sublimed_data_t>>;});
static_assert(serialization::deserialize_concept<true,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>);
static_assert(serialization::deserialize_concept<false,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>);
static_assert(serialization::serialize_concept<true,std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<Grib1CommonDataProperties>,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>>);
static_assert(serialization::serialize_concept<false,std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<Grib1CommonDataProperties>,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>>);
//std::unordered_map<path::Storage<false>,std::map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfoStruct<Data_t::METEO,Data_f::GRIB>>>>