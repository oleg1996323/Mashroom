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
#include "network/common/utility.h"
#include "grib/capitalize_data_info.h"
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
using namespace std::string_literals;

class SublimedGribDataInfo;

#include <string_view>
#include "definitions/path_process.h"
namespace fs = std::filesystem;
class GribDataInfo{
    public:
    using data_t = std::unordered_map<path::Storage<false>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<GribCapitalizeDataInfo>>>;
    using sublimed_data_t = std::unordered_map<path::Storage<false>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfo>>>;
    protected:
    data_t info_;
    ErrorCodeData err = ErrorCodeData::NONE_ERR;
    friend class Capitalize;
    friend class Integrity;
    friend class Extract;
    public:
    GribDataInfo() =default;
    GribDataInfo(const GribDataInfo& other):info_(other.info_){}
    GribDataInfo(GribDataInfo&& other):info_(std::move(other.info_)){}
    GribDataInfo(const data_t& info):
    info_(info){}
    GribDataInfo(data_t&& info):
    info_(std::move(info)){}
    template<typename CDP = CommonDataProperties, typename GCDI = GribCapitalizeDataInfo>
    void add_info(const path::Storage<false>& path,CDP&& cmn,GCDI&& cap_info){
        if constexpr(std::is_same_v<std::vector<GribCapitalizeDataInfo>,std::decay_t<GCDI>>){
            info_[path][std::make_shared<CommonDataProperties>(std::forward<CDP>(cmn))].append_range(std::forward<GCDI>(cap_info));
        }
        else
            info_[path][std::make_shared<CommonDataProperties>(std::forward<CDP>(cmn))].push_back(std::forward<GCDI>(cap_info));
    }
    void add_info(const path::Storage<false>& path, const GribMsgDataInfo& msg_info) noexcept;
    void add_info(const path::Storage<false>& path, GribMsgDataInfo&& msg_info) noexcept;
    ErrorCodeData error() const;
    const data_t& data() const;
    void swap(GribDataInfo& other) noexcept;
    //void read(const fs::path& path);
    //void write(const fs::path& dir);
    SublimedGribDataInfo sublime();
};
#include <unordered_set>
#include "definitions/path_process.h"
class SublimedGribDataInfo
{
    public:
    using sublimed_data_t = std::unordered_map<path::Storage<false>,
    std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfo>>>;
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

    private:
    sublimed_data_t info_;
    std::unordered_set<path::Storage<true>> paths_;
    public:
    SublimedGribDataInfo() = default;
    SublimedGribDataInfo(SublimedGribDataInfo&& other):
    info_(std::move(other.info_)){}
    SublimedGribDataInfo(const SublimedGribDataInfo&) = delete;
    const sublimed_data_t& data() const{
        return info_;
    }

    const std::unordered_set<path::Storage<true>>& paths() const{
        return paths_;
    }
    void add_data(SublimedGribDataInfo& grib_data){
        for(auto& [path,file_data]:grib_data.info_){
            auto found = info_.find(path);
            if(found==info_.end())
                info_[path].swap(file_data);
            else 
                info_[found->first].swap(file_data);
        }
    }
    void add_data(SublimedGribDataInfo::sublimed_data_t& grib_data){
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
    void add_data(SublimedGribDataInfo&& grib_data){
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
    void add_data(SublimedGribDataInfo::sublimed_data_t&& grib_data){
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
        SerializationEC operator()(const type& msg, std::vector<char>& buf) noexcept{
            SerializationEC err = SerializationEC::NONE;
            auto type_enum = __Data__::FORMAT::GRIB;
            serialize<NETWORK_ORDER>(type_enum,buf);
            size_t info_sz = 0;
            auto existing_paths = std::views::all(msg.info_)|
                                    std::views::filter([&info_sz](const auto& pair){
                                        const auto& pathstore = std::get<0>(pair);
                                        if(fs::exists(pathstore.path_) || 
                                        pathstore.type_==path::TYPE::HOST){
                                            ++info_sz;
                                            return true;
                                        }
                                        else return false;
                                    });
            err = serialize<NETWORK_ORDER>(info_sz,buf);
            for(const auto& [path,filedata]: existing_paths)
            {
                if(err==SerializationEC::NONE)
                    err = serialize<NETWORK_ORDER>(path,buf);
                if(err==SerializationEC::NONE)
                    err = serialize<NETWORK_ORDER>(filedata,buf);
            }
            return err;
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            SerializationEC err=SerializationEC::NONE;
            auto enum_type = __Data__::FORMAT::GRIB;
            err = deserialize<NETWORK_ORDER>(enum_type,buf);
            if(err==SerializationEC::NONE)
                err = deserialize<NETWORK_ORDER>(msg.info_,buf);
            return err;
        }
    };

    template<>
    struct Serial_size<SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        size_t operator()(const type& msg) noexcept{
            size_t info_sz = 0;
            auto existing_paths = std::ranges::copy_if_result(msg.info_,[&info_sz](const auto& pair){
                                        const auto& pathstore = std::get<0>(pair);
                                        if(fs::exists(pathstore.path_) || 
                                        pathstore.type_==path::TYPE::HOST){
                                            ++info_sz;
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
        constexpr size_t operator()(const type& msg) noexcept{
            return min_serial_size(__Data__::FORMAT::GRIB,msg.info_);
        }
    };

    template<>
    struct Max_serial_size<SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        constexpr size_t operator()(const type& msg) noexcept{
            return max_serial_size(__Data__::FORMAT::GRIB,msg.info_);
        }
    };
}

//@todo make possible serialization
static_assert(requires{requires std::ranges::range<GribDataInfo::sublimed_data_t>;});

static_assert(serialization::serialize_concept<true,std::pair<std::shared_ptr<CommonDataProperties> const, std::vector<SublimedDataInfo, std::allocator<SublimedDataInfo>>>>);
static_assert(serialization::serialize_concept<false,std::pair<std::shared_ptr<CommonDataProperties> const, std::vector<SublimedDataInfo, std::allocator<SublimedDataInfo>>>>);
static_assert(requires {requires serialization::serialize_concept<true,std::optional<GribDataInfo::sublimed_data_t>>;});
static_assert(requires {requires serialization::serialize_concept<false,std::optional<GribDataInfo::sublimed_data_t>>;});
static_assert(serialization::deserialize_concept<true,std::vector<SublimedDataInfo>>);
static_assert(serialization::deserialize_concept<false,std::vector<SublimedDataInfo>>);
static_assert(serialization::serialize_concept<true,std::unordered_map<path::Storage<false>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfo>>>>);
static_assert(serialization::serialize_concept<false,std::unordered_map<path::Storage<false>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfo>>>>);
//std::unordered_map<path::Storage<false>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfo>>>