#include "types/data_info.h"

void GribDataInfo::add_info(const GribMsgDataInfo& msg_info) noexcept{
    std::vector<GribCapitalizeDataInfo>& grib_data = info_[CommonDataProperties(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)];
    // int32_t int_time = get_epoch_time(&msg_info.date);
    // if(int_time<0)
    //     return;
    // for(GribCapitalizeDataInfo& grib_tmp:grib_data){
    //     if(grib_tmp.grid_data==msg_info.grid_data){ //if grids are equal
    //         if(grib_tmp.to==0 && grib_tmp.from==UINT32_MAX){
    //             grib_tmp.to = grib_tmp.from = int_time;
    //             grib_tmp.buf_pos_=msg_info.buf_pos_; //add buffer position
    //         }
    //         else if(int_time > grib_tmp.to) //extending interval
    //         {
    //             if(grib_tmp.discret == 0 || int_time-grib_tmp.to==grib_tmp.discret){
    //                 grib_tmp.to = int_time;
    //             }
    //             else
    //                 continue;
    //         }
    //         else if(int_time < grib_tmp.from) //extending interval
    //         {
    //             if(grib_tmp.discret == 0 || grib_tmp.from-int_time==grib_tmp.discret){
    //                 grib_tmp.from = int_time;
    //                 grib_tmp.buf_pos_= msg_info.buf_pos_; //update buffer position
    //             }
    //             else
    //                 continue;
    //         }
    //         else if(int_time<grib_tmp.to && int_time>grib_tmp.from){ //if value is in time interval
    //             assert(grib_tmp.discret!=0); //must be defined
    //             //int elements = (grib_tmp.to-grib_tmp.from)/grib_tmp.discret+1;
    //             if(int_time-grib_tmp.from==grib_tmp.to-int_time && grib_tmp.to-int_time>grib_tmp.discret)
    //                 grib_tmp.discret/=2;
    //             else continue;
    //         }
    //         return;
    //     }
    //     else continue;
    // }
    GribCapitalizeDataInfo data;
    data.to = data.from = msg_info.date;
    data.buf_pos_ = msg_info.buf_pos_;
    data.grid_data = msg_info.grid_data;
    grib_data.push_back(std::move(data));
}