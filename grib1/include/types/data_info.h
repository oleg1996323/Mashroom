#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include "aux_code/def.h"
#include "types/date.h"
#include "types/grid.h"
#include "types/data_suite.h"
#include "code_tables/table_0.h"
#include "sections/grid/grid.h"
#include "def.h"

STRUCT_BEG(CenterSubcenter)
{
    Organization center = WMO;
    uint8_t subcenter = 0;
}
STRUCT_END(CenterSubcenter)

template<>
struct std::hash<CenterSubcenter>{
    size_t operator()(CenterSubcenter cs){
        return std::hash<size_t>{}(cs.center<<8+cs.subcenter);
    }
};

STRUCT_BEG(GribMsgDataInfo)
{
    GridInfo grid_data;
    Date from;
    Date to;
    #ifdef __cplusplus
    std::set<uint8_t> parameters;
    uint8_t time_inc_ = 0;
    TimeFrame time_unit_ = TimeFrame::MINUTE;
    #else
    uint8_t* parameters;
    #endif
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)

    #ifdef __cplusplus
    bool contains_param(uint8_t param) const{
        return parameters.contains(param);
    }
    #endif
}
STRUCT_END(GribMsgDataInfo)

STRUCT_BEG(GribDataInfo)
{
    std::unordered_map<CenterSubcenter,std::vector<GribMsgDataInfo>> info_;
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)
    void add_info(const GridInfo& grid,const Date& from, const Date& to,TimeFrame t_unit,uint8_t time_increment,uint8_t parameter, Organization center, uint8_t subcenter) noexcept{
        CenterSubcenter tmp{center,subcenter};
        if(!info_.contains(tmp)){
            GribMsgDataInfo grib_tmp;
            grib_tmp.parameters.insert(parameter);
            grib_tmp.from = from;
            grib_tmp.to = to;
            grib_tmp.grid_data = grid;
            grib_tmp.time_unit_ = t_unit;
            grib_tmp.time_inc_ = time_increment;
            info_[tmp].push_back(std::move(grib_tmp));
        }
        else{
            std::vector<GribMsgDataInfo>& grib_data = info_.at(tmp);
            bool to_add = true;
            int is_date_interval_extension = 0;
            for(GribMsgDataInfo& grib_tmp:grib_data){
                if(t_unit!=grib_tmp.time_unit_ ||
                time_increment!=grib_tmp.time_inc_){
                        if(is_date_interval(&from,&grib_tmp.to,t_unit,time_increment))
                            is_date_interval_extension = -1;
                        else if(is_date_interval(&to,&grib_tmp.from,t_unit,time_increment))
                            is_date_interval_extension = 1;
                        else if(!date_equal(&grib_tmp.from,&from) && !date_equal(&grib_tmp.to,&to) &&
                        !date_equal(&grib_tmp.from,&to) && !date_equal(&grib_tmp.to,&from))
                            continue;
                }
                else{
                    if(is_date_interval_extension!=0){
                        if(grib_tmp.grid_data.extendable_by(grid)){
                            is_date_interval_extension = 0;
                            continue;
                        }
                        else if(is_date_interval_extension==-1)
                            grib_tmp.from = from;
                        else if(is_date_interval_extension==1)
                            grib_tmp.to = to;
                        else throw std::runtime_error("Unexpected function behavior");
                        to_add = false;
                        break;
                    }
                    else{
                        to_add = !grib_tmp.grid_data.extend(grid);
                        break;
                    }
                }
            }
            if(to_add){
                GribMsgDataInfo msg_info;
                msg_info.parameters.insert(parameter);
                msg_info.from = from;
                msg_info.to = to;
                msg_info.grid_data = grid;
                msg_info.time_unit_ = t_unit;
                msg_info.time_inc_ = time_increment;
                info_[tmp].push_back(std::move(msg_info));
            }
        }
    }

    void add_info(const GridInfo& grid,const Date& date, uint8_t parameter, Organization center, uint8_t subcenter) noexcept{
        CenterSubcenter tmp{center,subcenter};
        if(!info_.contains(tmp)){
            GribMsgDataInfo grib_tmp;
            grib_tmp.parameters.insert(parameter);
            grib_tmp.from = date;
            grib_tmp.to = date;
            grib_tmp.grid_data = grid;
            info_[tmp].push_back(std::move(grib_tmp));
        }
        else{
            std::vector<GribMsgDataInfo>& grib_data = info_.at(tmp);
            bool to_add = true;
            int is_date_interval_extension = 0;
            for(GribMsgDataInfo& grib_tmp:grib_data){
                if(grib_tmp.grid_data==grid){
                    
                }
                else{
                    if(is_date_interval_extension!=0){
                        if(grib_tmp.grid_data.extendable_by(grid)){
                            is_date_interval_extension = 0;
                            continue;
                        }
                        else if(is_date_interval_extension==-1)
                            grib_tmp.from = date;
                        else if(is_date_interval_extension==1)
                            grib_tmp.to = date;
                        else throw std::runtime_error("Unexpected function behavior");
                        to_add = false;
                        break;
                    }
                    else{
                        to_add = !grib_tmp.grid_data.extend(grid);
                        break;
                    }
                }
            }
            if(to_add){
                GribMsgDataInfo msg_info;
                msg_info.parameters.insert(parameter);
                msg_info.from = date;
                msg_info.to = date;
                msg_info.grid_data = grid;
                msg_info.time_unit_ = t_unit;
                msg_info.time_inc_ = time_increment;
                info_[tmp].push_back(std::move(msg_info));
            }
        }
    }

    //does not include possibility to separate time interval by time-point where grid was extended TODO:
    // void add_info(const GridInfo& grid,const Date& date, TimeFrame t_unit,uint8_t time_increment,uint8_t parameter, Organization center, uint8_t subcenter) noexcept{
    //     CenterSubcenter tmp{center,subcenter};
    //     if(!info_.contains(tmp)){
    //         GribMsgDataInfo grib_tmp;
    //         grib_tmp.parameters.insert(parameter);
    //         grib_tmp.from = date;
    //         grib_tmp.to = date;
    //         grib_tmp.grid_data = grid;
    //         grib_tmp.time_unit_ = t_unit;
    //         grib_tmp.time_inc_ = time_increment;
    //         info_[tmp].push_back(std::move(grib_tmp));
    //     }
    //     else{
    //         std::vector<GribMsgDataInfo>& grib_data = info_.at(tmp);
    //         bool to_add = true;
    //         int is_date_interval_extension = 0;
    //         for(GribMsgDataInfo& grib_tmp:grib_data){
    //             if(t_unit!=grib_tmp.time_unit_ ||
    //             time_increment!=grib_tmp.time_inc_){
    //                     if(is_date_interval(&date,&grib_tmp.to,t_unit,time_increment))
    //                         is_date_interval_extension = -1;
    //                     else if(is_date_interval(&date,&grib_tmp.from,t_unit,time_increment))
    //                         is_date_interval_extension = 1;
    //                     else if(!date_equal(&grib_tmp.from,&date) && !date_equal(&grib_tmp.to,&date))
    //                         continue;
    //             }
    //             else{
    //                 if(is_date_interval_extension!=0){
    //                     if(grib_tmp.grid_data.extendable_by(grid)){
    //                         is_date_interval_extension = 0;
    //                         continue;
    //                     }
    //                     else if(is_date_interval_extension==-1)
    //                         grib_tmp.from = date;
    //                     else if(is_date_interval_extension==1)
    //                         grib_tmp.to = date;
    //                     else throw std::runtime_error("Unexpected function behavior");
    //                     to_add = false;
    //                     break;
    //                 }
    //                 else{
    //                     to_add = !grib_tmp.grid_data.extend(grid);
    //                     break;
    //                 }
    //             }
    //         }
    //         if(to_add){
    //             GribMsgDataInfo msg_info;
    //             msg_info.parameters.insert(parameter);
    //             msg_info.from = date;
    //             msg_info.to = date;
    //             msg_info.grid_data = grid;
    //             msg_info.time_unit_ = t_unit;
    //             msg_info.time_inc_ = time_increment;
    //             info_[tmp].push_back(std::move(msg_info));
    //         }
    //     }
    // }
    // void add_info(const GribDataInfo& other){
        
    // }
}
STRUCT_END(GribDataInfo)