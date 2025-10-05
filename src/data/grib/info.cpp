#include "data/info.h"

namespace fs = std::filesystem;
void GribDataInfo::add_info(const path::Storage<false>& path, const GribMsgDataInfo& msg_info)  noexcept{
    info_[path][std::make_shared<CommonDataProperties<Data_t::METEO,Data_f::GRIB>>(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)]
    .emplace_back(IndexDataInfo<API::GRIB1>{
        msg_info.grid_data,
        msg_info.buf_pos_,
        msg_info.date,
        API::ErrorData::Code<API::GRIB1>::NONE_ERR});
}

void GribDataInfo::add_info(const path::Storage<false>& path, GribMsgDataInfo&& msg_info) noexcept{
    info_[path][std::make_shared<CommonDataProperties<Data_t::METEO,Data_f::GRIB>>(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)]
    .emplace_back(IndexDataInfo<API::GRIB1>{
        msg_info.grid_data,
        msg_info.buf_pos_,
        msg_info.date,
        API::ErrorData::Code<API::GRIB1>::NONE_ERR});
}

API::ErrorData::Code<API::GRIB1>::value GribDataInfo::error() const{
    return err;
}
const GribDataInfo::data_t& GribDataInfo::data() const {
    return info_;
}
void GribDataInfo::swap(GribDataInfo& other) noexcept{
    std::swap(info_,other.info_);
}
#include "sys/error_print.h"
#include "sections/section_1.h"
using namespace std::string_literals;

SublimedGribDataInfo GribDataInfo::sublime(){
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
            assert(std::is_sorted(data_seq.begin(),data_seq.end(),[](const IndexDataInfo<API::GRIB1>& lhs,const IndexDataInfo<API::GRIB1>& rhs)
            {
                if(lhs.err!=API::ErrorData::Code<API::GRIB1>::NONE_ERR)
                    return false;
                if(std::hash<std::optional<GridInfo>>{}(lhs.grid_data)<std::hash<std::optional<GridInfo>>{}(rhs.grid_data))
                    return true;
                else
                    return lhs.date_time<rhs.date_time;   
            }));
            for(int i=0;i<data_seq.size();++i){
                if(data_seq.at(i).err!=API::ErrorData::Code<API::GRIB1>::NONE_ERR)
                    continue;
                if(data_seq_tmp.empty()){
                    data_seq_tmp.emplace_back(GribSublimedDataInfoStruct{.grid_data_ = data_seq.at(i).grid_data,
                                                                        .buf_pos_={},
                                                                        .sequence_time_={.interval_=
                                                                            {
                                                                                .from_ = data_seq.at(i).date_time,
                                                                                .to_ = data_seq.at(i).date_time
                                                                            },
                                                                            .discret_={}}})
                                                                        .buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    continue;
                }
                if(data_seq_tmp.back().sequence_time_.discret_!=std::chrono::system_clock::duration(0)){
                    if(data_seq.at(i).date_time==data_seq_tmp.back().sequence_time_.interval_.to_+data_seq_tmp.back().sequence_time_.discret_){
                        data_seq_tmp.back().sequence_time_.interval_.to_=data_seq.at(i).date_time;
                        data_seq_tmp.back().buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    }
                    else if(data_seq.at(i).date_time==data_seq_tmp.back().sequence_time_.interval_.from_-data_seq_tmp.back().sequence_time_.discret_){
                        data_seq_tmp.back().sequence_time_.interval_.from_=data_seq.at(i).date_time;
                        data_seq_tmp.back().buf_pos_.insert(
                        data_seq_tmp.back().buf_pos_.begin(),data_seq.at(i).buf_pos_);
                    }
                    else{
                        data_seq_tmp.emplace_back(GribSublimedDataInfoStruct
                                                                            {.grid_data_ = data_seq.at(i).grid_data,
                                                                            .buf_pos_={},
                                                                            .sequence_time_={.interval_={.from_=data_seq.at(i).date_time,.to_=data_seq.at(i).date_time},
                                                                            .discret_={}}})
                                                                            .buf_pos_.push_back(data_seq.at(i).buf_pos_);
                        continue;
                    }
                }
                else{
                    assert(data_seq_tmp.back().sequence_time_.interval_.from_==data_seq_tmp.back().sequence_time_.interval_.to_);
                    if(data_seq.at(i).date_time<data_seq_tmp.back().sequence_time_.interval_.from_){
                        data_seq_tmp.back().sequence_time_.interval_.from_=data_seq.at(i).date_time;
                        data_seq_tmp.back().sequence_time_.discret_ = data_seq_tmp.back().sequence_time_.interval_.to_-data_seq_tmp.back().sequence_time_.interval_.from_;
                        data_seq_tmp.back().buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    }
                    else if(data_seq.at(i).date_time>data_seq_tmp.back().sequence_time_.interval_.from_){
                        data_seq_tmp.back().sequence_time_.interval_.to_=data_seq.at(i).date_time;
                        data_seq_tmp.back().sequence_time_.discret_ = data_seq_tmp.back().sequence_time_.interval_.to_-data_seq_tmp.back().sequence_time_.interval_.from_;
                        data_seq_tmp.back().buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    }
                }
            }
        }
    }
    returned.add_data(result);
    return returned;
}