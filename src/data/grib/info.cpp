#include "data/info.h"

namespace fs = std::filesystem;
void GribProxyDataInfo::add_info(const path::Storage<false>& path, const GribMsgDataInfo& msg_info)  noexcept{
    CommonDataProperties<Data_t::METEO,Data_f::GRIB_v1> cmn(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter);
    auto found = info_[path].find(cmn);
    auto index = IndexDataInfo<API::GRIB1>{
            msg_info.grid_data,
            msg_info.buf_pos_,
            msg_info.date,
            API::ErrorData::Code<API::GRIB1>::NONE_ERR};
    if(found==info_[path].end())
        info_[path][std::make_shared<CommonDataProperties<Data_t::METEO,Data_f::GRIB_v1>>(std::move(cmn))]
        .emplace_back(std::move(index));
    else found->second.emplace_back(std::move(index));
}

void GribProxyDataInfo::add_info(const path::Storage<false>& path, GribMsgDataInfo&& msg_info) noexcept{
    CommonDataProperties<Data_t::METEO,Data_f::GRIB_v1> cmn(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter);
    auto found = info_[path].find(cmn);
    auto index = IndexDataInfo<API::GRIB1>{
            .grid_data=std::move(msg_info.grid_data),
            .buf_pos_ = msg_info.buf_pos_,
            .date_time = msg_info.date,
            .err = API::ErrorData::Code<API::GRIB1>::NONE_ERR};
    if(found==info_[path].end())
        info_[path][std::make_shared<CommonDataProperties<Data_t::METEO,Data_f::GRIB_v1>>(std::move(cmn))]
        .emplace_back(std::move(index));
    else{
        found->second.emplace_back(std::move(index));
        assert(found->first && *found->first==cmn);
    }
}

API::ErrorData::Code<API::GRIB1>::value GribProxyDataInfo::error() const{
    return err;
}
const GribProxyDataInfo::data_t& GribProxyDataInfo::data() const {
    return info_;
}
void GribProxyDataInfo::swap(GribProxyDataInfo& other) noexcept{
    std::swap(info_,other.info_);
}
#include "sys/error_print.h"
#include "sections/section_1.h"
using namespace std::string_literals;

SublimedGribDataInfo GribProxyDataInfo::sublime(){
    SublimedGribDataInfo returned;
    sublimed_data_t result;
    for(auto& [path,path_data]:info_){
        auto& data_t_tmp = result[path];
        for(auto& [cmn_d,data_seq]:path_data){
            auto& data_seq_tmp = data_t_tmp[cmn_d];
            //placing without grid to the beginning
            //errorness structures to the end
            std::ranges::sort(data_seq,[](const IndexDataInfo<API::GRIB1>& lhs,const IndexDataInfo<API::GRIB1>& rhs)->bool
            {
                if(lhs.err!=API::ErrorData::Code<API::GRIB1>::NONE_ERR)
                    return false;
                if(std::hash<std::optional<GridInfo>>{}(lhs.grid_data)<std::hash<std::optional<GridInfo>>{}(rhs.grid_data))
                    return true;
                else
                    return lhs.date_time<rhs.date_time;   
            });
            auto iter = std::unique(data_seq.begin(),data_seq.end());
            data_seq.erase(iter,data_seq.end());
            for(int i=0;i<data_seq.size();++i){
                if(data_seq.at(i).err!=API::ErrorData::Code<API::GRIB1>::NONE_ERR)
                    continue;
                if(data_seq_tmp.empty()){
                    data_seq_tmp.emplace_back(GribSublimedDataInfoStruct{.grid_data_ = data_seq.at(i).grid_data,
                                                                        .buf_pos_={},
                                                                        .sequence_time_=TimeSequence(data_seq.at(i).date_time)
                                                                        })
                                                                        .buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    continue;
                }
                else{
                    if(data_seq_tmp.back().sequence_time_.push_time_after(data_seq.at(i).date_time))
                        data_seq_tmp.back().buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    else{
                        data_seq_tmp.emplace_back(GribSublimedDataInfoStruct{.grid_data_ = data_seq.at(i).grid_data,
                                                                            .buf_pos_={},
                                                                            .sequence_time_=TimeSequence(data_seq.at(i).date_time)
                                                                            })
                                                                            .buf_pos_.push_back(data_seq.at(i).buf_pos_);
                        continue;
                    }
                }
            }
        }
    }
    returned.add_data(std::move(result));
    return returned;
}