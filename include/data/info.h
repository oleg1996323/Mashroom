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
#include "def.h"
#include "types/time_interval.h"
#include "common.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "msg.h"
#include "cmd_parse/cmd_translator.h"
#include "sys/error_print.h"
#include <sys/error_code.h>
#include <cassert>
#include <cstring>
#include <netinet/in.h>
#include "network/common/utility.h"
#include "grib/capitalize_data_info.h"
#include "sublimed_info.h"
#include "serialization.h"
using namespace std::string_literals;

class SublimedGribDataInfo;

#include <string_view>
#include "definitions/path_process.h"
namespace fs = std::filesystem;
class GribDataInfo{
    public:
    using data_t = std::unordered_map<path::Storage<false>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<GribCapitalizeDataInfo>>>;
    using sublimed_data_t = std::unordered_map<path::Storage<true>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfo>>>;
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
    using sublimed_data_t = std::unordered_map<path::Storage<true>,
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
    std::unordered_set<path::Storage<false>> paths_;
    public:
    SublimedGribDataInfo() = default;
    SublimedGribDataInfo(SublimedGribDataInfo&& other):
    info_(std::move(other.info_)),paths_(std::move(other.paths_)){}
    SublimedGribDataInfo(const SublimedGribDataInfo&) = delete;
    const sublimed_data_t& data() const{
        return info_;
    }
    // void serialize(std::ofstream& file);
    // void deserialize(std::ifstream& file);
    const std::unordered_set<path::Storage<false>>& paths() const{
        return paths_;
    }
    // void serialize(std::vector<char>& buf) const;
    // void deserialize(std::vector<char>& buf) const;
    void add_data(SublimedGribDataInfo& grib_data){
        for(auto& [path,file_data]:grib_data.info_){
            auto found = paths_.find(path);
            if(found==paths_.end())
                info_[*paths_.emplace(path).first].swap(file_data);
            else 
                info_[*paths_.insert(*found).first].swap(file_data);
        }
    }
    void add_data(SublimedGribDataInfo::sublimed_data_t& grib_data){
        for(auto& [path,file_data]:grib_data){
            auto found = paths_.find(path);
            if(found==paths_.end())
                info_[*paths_.emplace(path).first].swap(file_data);
            else 
                info_[*paths_.insert(*found).first].swap(file_data);
        }
    }
    void add_data(SublimedGribDataInfo&& grib_data){
        for(auto& [path,file_data]:grib_data.info_){
            auto found = paths_.find(path);
            if(found==paths_.end())
                info_[*paths_.emplace(path).first].swap(file_data);
            else 
                info_[*paths_.insert(*found).first].swap(file_data);
        }
    }
    void add_data(SublimedGribDataInfo::sublimed_data_t&& grib_data){
        for(auto& [path,file_data]:grib_data){
            auto found = paths_.find(path);
            if(found==paths_.end()){
                info_[*paths_.emplace(path).first].swap(file_data);
            }
            else{
                info_[*paths_.insert(*found).first].swap(file_data);
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
            serialize<NETWORK_ORDER>(__Data__::FORMAT::GRIB,buf);
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
            serialize<NETWORK_ORDER>(info_sz,buf);
            for(const auto& [path,filedata]: existing_paths)
            {
                serialize<NETWORK_ORDER>(path,buf,path.type_,path.path_);
                serialize<NETWORK_ORDER>(filedata,buf);
            }
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SublimedGribDataInfo>{
        using type = SublimedGribDataInfo;
        SerializationEC operator()(type& msg, std::span<const char> buf) noexcept{
            __Data__::FORMAT tokens_f;
            deserialize<NETWORK_ORDER>(tokens_f,buf);
            if(SerializationEC err = deserialize<NETWORK_ORDER>(msg.info_,buf);err!=SerializationEC::NONE)
                return err;
            else return SerializationEC::NONE;
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