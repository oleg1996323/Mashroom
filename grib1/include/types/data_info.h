#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <list>
#include "aux_code/def.h"
#include "types/date.h"
#include "types/grid.h"
#include "data_common.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "types/data_msg.h"

STRUCT_BEG(GribCapitalizeDataInfo)
{
    GridInfo grid_data;
    ptrdiff_t buf_pos_;
    uint32_t from = UINT32_MAX;
    uint32_t to = 0;
    uint32_t discret = 0;
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)
}
STRUCT_END(GribCapitalizeDataInfo)

class GribDataInfo{
    public:
    using data_t = std::unordered_map<CommonDataProperties,std::vector<GribCapitalizeDataInfo>>;
    private:
    data_t info_;
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)
    friend class Capitalize;
    friend class Check;
    public:
    GribDataInfo() =default;
    GribDataInfo(const data_t& info):
    info_(info){}
    GribDataInfo(data_t&& info):
    info_(std::move(info)){}
    void add_info(const CommonDataProperties& cmn,const GribCapitalizeDataInfo& cap_info);
    void add_info(const CommonDataProperties& cmn,const std::vector<GribCapitalizeDataInfo>& cap_info);
    void add_info(CommonDataProperties&& cmn,GribCapitalizeDataInfo&& cap_info);
    void add_info(CommonDataProperties&& cmn,std::vector<GribCapitalizeDataInfo>&& cap_info);
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
                if(current->discret!=0){
                    if(data_seq[i].discret==0)
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
                    if(data_seq[i].discret==0){
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