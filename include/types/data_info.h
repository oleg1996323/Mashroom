#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <list>
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
    std::chrono::system_clock::time_point from = std::chrono::system_clock::time_point::max();
    std::chrono::system_clock::time_point to = std::chrono::system_clock::time_point::min();
    std::chrono::system_clock::duration discret = std::chrono::system_clock::duration(0);
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)
};

class GribDataInfo{
    public:
    using data_t = std::unordered_map<CommonDataProperties,std::vector<GribCapitalizeDataInfo>>;
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
    void add_info(const GribMsgDataInfo& msg_info) noexcept;
    ErrorCodeData error() const{
        return err;
    }
    const data_t& data() const {
        return info_;
    }
    void swap(GribDataInfo& other) noexcept{
        std::swap(info_,other.info_);
    }
    void sublime(){
        data_t tmp;
        for(auto& [cmn_d,data_seq]:info_){
            std::sort(data_seq.begin(),data_seq.end(),[](const GribCapitalizeDataInfo& lhs,const GribCapitalizeDataInfo& rhs)
            {
                return lhs.from<rhs.from;
            });
            decltype(data_seq)::value_type* current;
            if(!data_seq.empty())
                current = &tmp[cmn_d].emplace_back(data_seq.front());
            for(int i=1;i<data_seq.size();++i){
                if(current->discret!=std::chrono::system_clock::duration(0)){
                    if(data_seq[i].discret==std::chrono::system_clock::duration(0))
                        if(data_seq[i].from==current->to+current->discret)
                            current->to=data_seq[i].to;
                        else
                            current = &tmp[cmn_d].emplace_back(data_seq[i]);
                    else{
                        if(data_seq[i].from==current->to+current->discret && data_seq[i].discret==current->discret)
                            current->to=data_seq[i].to;
                        else
                            current = &tmp[cmn_d].emplace_back(data_seq[i]);
                    }
                }
                else{
                    if(data_seq[i].discret==std::chrono::system_clock::duration(0)){
                        current->to = data_seq[i].to;
                        current->discret = current->to-current->from;
                    }
                    else{
                        if(data_seq[i].from-data_seq[i].discret==current->to){
                            current->to=data_seq[i].to;
                            current->discret = data_seq[i].discret;
                        }
                        else{
                            //TODO: использовать while с прогнозированием дальнейших дискретностей. При необходимости, разделить интервал 
                            current = &tmp[cmn_d].emplace_back(data_seq[i]);
                        }
                    }
                }//intervals may intersect (check if from is less than current to)
            }
        }
    }
};