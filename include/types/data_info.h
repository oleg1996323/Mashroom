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
    //TODO: make sorted by date-time vector with buf_pos and date-time
    //erase discret and to. Make representation_type instead of GridInfo (very hard)
    std::chrono::system_clock::time_point date_time = std::chrono::system_clock::time_point::max();
    RepresentationType rep_t;
    ErrorCodeData err=NONE_ERR;
};

struct SublimedGribCapitalizeDataInfo
{
    std::optional<GridInfo> grid_data;
    ptrdiff_t buf_pos_;
    //TODO: make sorted by date-time vector with buf_pos and date-time
    //erase discret and to. Make representation_type instead of GridInfo (very hard)
    std::chrono::system_clock::time_point from = std::chrono::system_clock::time_point::max();
    std::chrono::system_clock::time_point to = std::chrono::system_clock::time_point::max();
    std::chrono::system_clock::duration discret = std::chrono::system_clock::duration(0);
    RepresentationType rep_t;
};

namespace fs = std::filesystem;
class GribDataInfo{
    public:
    using data_t = std::unordered_map<fs::path,std::unordered_map<CommonDataProperties,std::vector<GribCapitalizeDataInfo>>>;
    using sublimed_data_t = std::unordered_map<fs::path,std::unordered_map<CommonDataProperties,std::vector<SublimedGribCapitalizeDataInfo>>>;
    private:
    data_t info_;

    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)
    friend class Capitalize;
    friend class Check;
    friend class Extract;
    public:
    GribDataInfo() =default;
    GribDataInfo(const data_t& info):
    info_(info){}
    GribDataInfo(data_t&& info):
    info_(std::move(info)){}
    template<typename CDP = CommonDataProperties, typename GCDI = GribCapitalizeDataInfo>
    void add_info(CDP&& cmn,GCDI&& cap_info){
        if constexpr(std::is_same_v<std::vector<GribCapitalizeDataInfo>,std::decay_t<GCDI>>)
            info_[std::forward<CDP>(cmn)].append_range(std::forward<GCDI>(cap_info));
        else
            info_[std::forward<CDP>(cmn)].push_back(std::forward<GCDI>(cap_info));
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
                std::ranges::sort(data_seq,[](const GribCapitalizeDataInfo& lhs,const GribCapitalizeDataInfo& rhs)
                {
                    return lhs.date_time<rhs.date_time;
                });
                for(int i=1;i<data_seq.size();++i){
                    if(data_seq[i].err!=ErrorCodeData::NONE_ERR)
                        continue;
                    if(data_seq_tmp.empty()){
                        data_seq_tmp.emplace_back(SublimedGribCapitalizeDataInfo{.grid_data = data_seq[i].grid_data,
                                                                            .buf_pos_ =  data_seq[i].buf_pos_,
                                                                            .from = data_seq[i].date_time,
                                                                            .to = data_seq[i].date_time,
                                                                            .discret={},
                                                                            .rep_t = data_seq[i].rep_t});
                        continue;
                    }
                    if(data_seq_tmp.back().discret!=std::chrono::system_clock::duration(0)){
                        if(data_seq[i].date_time==data_seq_tmp.back().to+data_seq_tmp.back().discret)
                            data_seq_tmp.back().to=data_seq[i].date_time;
                        else if(data_seq_tmp.back().from-data_seq_tmp.back().discret==data_seq[i].date_time){
                            data_seq_tmp.back().from=data_seq[i].date_time;
                        }
                        else{
                            data_seq_tmp.emplace_back(SublimedGribCapitalizeDataInfo{.grid_data = data_seq[i].grid_data,
                                                                                .buf_pos_ =  data_seq[i].buf_pos_,
                                                                                .from = data_seq[i].date_time,
                                                                                .to = data_seq[i].date_time,
                                                                                .discret={},
                                                                                .rep_t = data_seq[i].rep_t});
                            continue;
                        }
                    }
                    else{
                        if(data_seq[i].date_time>data_seq_tmp.back().to){
                            data_seq_tmp.back().to=data_seq[i].date_time;
                            data_seq_tmp.back().discret = data_seq_tmp.back().to-data_seq_tmp.back().from;
                        }
                        else if(data_seq_tmp.back().from-data_seq_tmp.back().discret==data_seq[i].date_time){
                            data_seq_tmp.back().from=data_seq[i].date_time;
                            data_seq_tmp.back().discret = data_seq_tmp.back().to-data_seq_tmp.back().from;
                        }
                        else{
                            std::cout<<"Duplicate"<<std::endl;
                        }
                        continue;
                    }
                }
            }
        }
        return result;
    }
};