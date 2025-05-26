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
#include "aux_code/def.h"
#include "types/time_interval.h"
#include "common.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "data/msg.h"
#include "cmd_parse/cmd_translator.h"
#include "sys/error_print.h"
#include <sys/error_code.h>
#include <def.h>
#include <grib1/include/def.h>
using namespace std::string_literals;


struct GribCapitalizeDataInfo
{
    std::optional<GridInfo> grid_data;
    ptrdiff_t buf_pos_;
    std::chrono::system_clock::time_point date_time = std::chrono::system_clock::time_point::max();
    ErrorCodeData err=NONE_ERR;
};

struct SublimedDataInfo
{
    std::optional<GridInfo> grid_data;
    std::vector<ptrdiff_t> buf_pos_;
    //TODO: make sorted by date-time vector with buf_pos and date-time
    //erase discret and to. Make representation_type instead of GridInfo (very hard)
    std::chrono::system_clock::time_point from = std::chrono::system_clock::time_point::max();
    std::chrono::system_clock::time_point to = std::chrono::system_clock::time_point::min();
    std::chrono::system_clock::duration discret = std::chrono::system_clock::duration(0);

    void serialize(std::ofstream& file) const{
        file.write(reinterpret_cast<const char*>(&grid_data),sizeof(decltype(grid_data)));
        size_t data_sz = buf_pos_.size();
        file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));
        file.write(reinterpret_cast<const char*>(buf_pos_.data()),sizeof(decltype(buf_pos_)::value_type)*data_sz);
        file.write(reinterpret_cast<const char*>(&from),sizeof(decltype(from)));
        file.write(reinterpret_cast<const char*>(&to),sizeof(decltype(to)));
        file.write(reinterpret_cast<const char*>(&discret),sizeof(decltype(discret)));
    }

    void deserialize(std::ifstream& file){
        file.read(reinterpret_cast<char*>(&grid_data),sizeof(decltype(grid_data)));
        size_t data_sz=0;
        file.read(reinterpret_cast<char*>(&data_sz),sizeof(data_sz));
        buf_pos_.clear();
        buf_pos_.resize(data_sz);
        file.read(reinterpret_cast<char*>(buf_pos_.data()),sizeof(decltype(buf_pos_)::value_type)*data_sz);
        file.read(reinterpret_cast<char*>(&from),sizeof(decltype(from)));
        file.read(reinterpret_cast<char*>(&to),sizeof(decltype(to)));
        file.read(reinterpret_cast<char*>(&discret),sizeof(decltype(discret)));
    }

    // std::vector<GribCapitalizeDataInfo> desublime(){
    //     std::vector<GribCapitalizeDataInfo> result;
    //     if(discret!=std::chrono::system_clock::duration(0))
    //         result.resize((to-from)/discret);
    //     else return {GribCapitalizeDataInfo{.grid_data = this->grid_data,.buf_pos_ = this->buf_pos_.back(),.date_time=this->from,.err=NONE_ERR}};
    // }
};

class SublimedGribDataInfo;

#include <string_view>
#include <path_process.h>
namespace fs = std::filesystem;
class GribDataInfo{
    public:
    using data_t = std::unordered_map<path::Storage<false>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<GribCapitalizeDataInfo>>>;
    using sublimed_data_t = std::unordered_map<path::Storage<true>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfo>>>;
    protected:
    data_t info_;
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)
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
#include <path_process.h>
class SublimedGribDataInfo
{
    public:
    using sublimed_data_t = std::unordered_map<path::Storage<true>,std::unordered_map<std::shared_ptr<CommonDataProperties>,std::vector<SublimedDataInfo>>>;
    
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
    void serialize(std::ofstream& file);
    void deserialize(std::ifstream& file);
    const std::unordered_set<path::Storage<false>>& paths() const{
        return paths_;
    }
    void serialize(std::vector<char>& buf);
    void deserialize(std::vector<char>& buf);
    const std::unordered_set<path::Storage<false>>& paths() const{
        return paths_;
    }
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