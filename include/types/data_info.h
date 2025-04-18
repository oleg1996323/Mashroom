#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include "aux_code/def.h"
#include "types/date.h"
#include "data_common.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "data_msg.h"

struct GribCapitalizeDataInfo
{
    std::optional<GridInfo> grid_data;
    ptrdiff_t buf_pos_;
    std::chrono::system_clock::time_point date_time = std::chrono::system_clock::time_point::max();
    ErrorCodeData err=NONE_ERR;
};

struct SublimedGribCapitalizeDataInfo
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
        size_t data_sz = buf_pos_.size();
        file.read(reinterpret_cast<char*>(&data_sz),sizeof(data_sz));
        buf_pos_.clear();
        buf_pos_.resize(data_sz);
        file.read(reinterpret_cast<char*>(buf_pos_.data()),sizeof(decltype(buf_pos_)::value_type)*data_sz);
        file.read(reinterpret_cast<char*>(&from),sizeof(decltype(from)));
        file.read(reinterpret_cast<char*>(&to),sizeof(decltype(to)));
        file.read(reinterpret_cast<char*>(&discret),sizeof(decltype(discret)));
    }

    std::vector<GribCapitalizeDataInfo> desublime(){
        std::vector<GribCapitalizeDataInfo> result;
        if(discret!=std::chrono::system_clock::duration(0))
            result.resize((to-from)/discret);
        else return {GribCapitalizeDataInfo{.grid_data = this->grid_data,.buf_pos_ = this->buf_pos_.back(),.date_time=this->from,.err=NONE_ERR}};
    }
};

namespace fs = std::filesystem;
class GribDataInfo{
    public:
    using data_t = std::unordered_map<fs::path,std::unordered_map<CommonDataProperties,std::vector<GribCapitalizeDataInfo>>>;
    using sublimed_data_t = std::unordered_map<fs::path,std::unordered_map<CommonDataProperties,std::vector<SublimedGribCapitalizeDataInfo>>>;
    protected:
    data_t info_;

    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)
    friend class Capitalize;
    friend class Check;
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
    void add_info(const fs::path& filename,CDP&& cmn,GCDI&& cap_info){
        if constexpr(std::is_same_v<std::vector<GribCapitalizeDataInfo>,std::decay_t<GCDI>>)
            info_[filename][std::forward<CDP>(cmn)].append_range(std::forward<GCDI>(cap_info));
        else
            info_[filename][std::forward<CDP>(cmn)].push_back(std::forward<GCDI>(cap_info));
    }
    void add_info(const fs::path& file_name, const GribMsgDataInfo& msg_info) noexcept;
    ErrorCodeData error() const{
        return err;
    }
    const data_t& data() const {
        return info_;
    }
    void swap(GribDataInfo& other) noexcept{
        std::swap(info_,other.info_);
    }
    sublimed_data_t sublime(){
        sublimed_data_t result;
        for(auto& [file_name,file_data]:info_){
            auto& data_t_tmp = result[file_name];
            for(auto& [cmn_d,data_seq]:file_data){
                auto& data_seq_tmp = data_t_tmp[cmn_d];
                //placing without grid to the beginning
                //errorness structures to the end
                std::ranges::sort(data_seq,[](const GribCapitalizeDataInfo& lhs,const GribCapitalizeDataInfo& rhs)
                {
                    if(lhs.err!=ErrorCodeData::NONE_ERR)
                        return false;
                    if(std::hash<std::optional<GridInfo>>{}(lhs.grid_data)<std::hash<std::optional<GridInfo>>{}(rhs.grid_data))
                        return true;
                    else
                        return lhs.date_time<rhs.date_time;   
                });
                for(int i=1;i<data_seq.size();++i){
                    if(data_seq[i].err!=ErrorCodeData::NONE_ERR)
                        continue;
                    if(data_seq_tmp.empty()){
                        data_seq_tmp.emplace_back(SublimedGribCapitalizeDataInfo{.grid_data = data_seq[i].grid_data,
                                                                            .buf_pos_={},
                                                                            .from = data_seq[i].date_time,
                                                                            .to = data_seq[i].date_time,
                                                                            .discret={}})
                                                                            .buf_pos_.push_back(data_seq[i].buf_pos_);
                        continue;
                    }
                    if(data_seq_tmp.back().discret!=std::chrono::system_clock::duration(0)){
                        if(data_seq[i].date_time==data_seq_tmp.back().to+data_seq_tmp.back().discret){
                            data_seq_tmp.back().to=data_seq[i].date_time;
                            data_seq_tmp.back().buf_pos_.push_back(data_seq[i].buf_pos_);
                        }
                        else if(data_seq_tmp.back().from-data_seq_tmp.back().discret==data_seq[i].date_time){
                            data_seq_tmp.back().from=data_seq[i].date_time;
                            data_seq_tmp.back().buf_pos_.insert(
                                data_seq_tmp.back().buf_pos_.begin(),data_seq[i].buf_pos_);
                        }
                        else{
                            data_seq_tmp.emplace_back(SublimedGribCapitalizeDataInfo{.grid_data = data_seq[i].grid_data,
                                                                                .buf_pos_={},
                                                                                .from = data_seq[i].date_time,
                                                                                .to = data_seq[i].date_time,
                                                                                .discret={}})
                                                                                .buf_pos_.push_back(data_seq[i].buf_pos_);
                            continue;
                        }
                    }
                    else{
                        if(data_seq[i].date_time>data_seq_tmp.back().to){
                            data_seq_tmp.back().to=data_seq[i].date_time;
                            data_seq_tmp.back().discret = data_seq_tmp.back().to-data_seq_tmp.back().from;
                            data_seq_tmp.back().buf_pos_.push_back(data_seq[i].buf_pos_);
                        }
                        else if(data_seq_tmp.back().from-data_seq_tmp.back().discret==data_seq[i].date_time){
                            data_seq_tmp.back().from=data_seq[i].date_time;
                            data_seq_tmp.back().discret = data_seq_tmp.back().to-data_seq_tmp.back().from;
                            data_seq_tmp.back().buf_pos_.push_back(data_seq[i].buf_pos_);
                        }
                        else{
                            std::cout<<"Duplicate"<<std::endl;
                        }
                    }
                }
            }
        }
        return result;
    }
};