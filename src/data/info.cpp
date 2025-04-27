#include "data/info.h"

namespace fs = std::filesystem;
void GribDataInfo::add_info(const fs::path& file_name, const GribMsgDataInfo& msg_info) noexcept{
    info_[file_name][std::make_shared<CommonDataProperties>(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)]
    .emplace_back(GribCapitalizeDataInfo{
        msg_info.grid_data,
        msg_info.buf_pos_,
        msg_info.date,
        ErrorCodeData::NONE_ERR});
}

void GribDataInfo::add_info(const fs::path& file_name, GribMsgDataInfo&& msg_info) noexcept{
    info_[file_name][std::make_shared<CommonDataProperties>(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)]
    .emplace_back(GribCapitalizeDataInfo{
        msg_info.grid_data,
        msg_info.buf_pos_,
        msg_info.date,
        ErrorCodeData::NONE_ERR});
}

ErrorCodeData GribDataInfo::error() const{
    return err;
}
const GribDataInfo::data_t& GribDataInfo::data() const {
    return info_;
}
void GribDataInfo::swap(GribDataInfo& other) noexcept{
    std::swap(info_,other.info_);
}
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "sections/section_1.h"
using namespace std::string_literals;

SublimedGribDataInfo GribDataInfo::sublime(){
    SublimedGribDataInfo returned;
    sublimed_data_t result;
    for(auto& [file_name,file_data]:info_){
        auto& data_t_tmp = result[file_name.c_str()];
        for(auto& [cmn_d,data_seq]:file_data){
            auto& data_seq_tmp = data_t_tmp[cmn_d];
            //placing without grid to the beginning
            //errorness structures to the end
            std::ranges::sort(data_seq,[](const GribCapitalizeDataInfo& lhs,const GribCapitalizeDataInfo& rhs)->bool
            {
                if(lhs.err!=ErrorCodeData::NONE_ERR)
                    return false;
                if(std::hash<std::optional<GridInfo>>{}(lhs.grid_data)<std::hash<std::optional<GridInfo>>{}(rhs.grid_data))
                    return true;
                else
                    return lhs.date_time<rhs.date_time;   
            });
            assert(std::is_sorted(data_seq.begin(),data_seq.end(),[](const GribCapitalizeDataInfo& lhs,const GribCapitalizeDataInfo& rhs)
            {
                if(lhs.err!=ErrorCodeData::NONE_ERR)
                    return false;
                if(std::hash<std::optional<GridInfo>>{}(lhs.grid_data)<std::hash<std::optional<GridInfo>>{}(rhs.grid_data))
                    return true;
                else
                    return lhs.date_time<rhs.date_time;   
            }));
            for(int i=0;i<data_seq.size();++i){
                if(data_seq.at(i).err!=ErrorCodeData::NONE_ERR)
                    continue;
                if(data_seq_tmp.empty()){
                    data_seq_tmp.emplace_back(SublimedDataInfo{.grid_data = data_seq.at(i).grid_data,
                                                                        .buf_pos_={},
                                                                        .from = data_seq.at(i).date_time,
                                                                        .to = data_seq.at(i).date_time,
                                                                        .discret={}})
                                                                        .buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    continue;
                }
                if(data_seq_tmp.back().discret!=std::chrono::system_clock::duration(0)){
                    if(data_seq.at(i).date_time==data_seq_tmp.back().to+data_seq_tmp.back().discret){
                        data_seq_tmp.back().to=data_seq.at(i).date_time;
                        data_seq_tmp.back().buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    }
                    else if(data_seq.at(i).date_time==data_seq_tmp.back().from-data_seq_tmp.back().discret){
                        data_seq_tmp.back().from=data_seq.at(i).date_time;
                        data_seq_tmp.back().buf_pos_.insert(
                        data_seq_tmp.back().buf_pos_.begin(),data_seq.at(i).buf_pos_);
                    }
                    else{
                        data_seq_tmp.emplace_back(SublimedDataInfo
                                                                            {.grid_data = data_seq.at(i).grid_data,
                                                                            .buf_pos_={},
                                                                            .from = data_seq.at(i).date_time,
                                                                            .to = data_seq.at(i).date_time,
                                                                            .discret={}})
                                                                            .buf_pos_.push_back(data_seq.at(i).buf_pos_);
                        continue;
                    }
                }
                else{
                    assert(data_seq_tmp.back().from==data_seq_tmp.back().to);
                    if(data_seq.at(i).date_time<data_seq_tmp.back().from){
                        data_seq_tmp.back().from=data_seq.at(i).date_time;
                        data_seq_tmp.back().discret = data_seq_tmp.back().to-data_seq_tmp.back().from;
                        data_seq_tmp.back().buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    }
                    else if(data_seq.at(i).date_time>data_seq_tmp.back().from){
                        data_seq_tmp.back().to=data_seq.at(i).date_time;
                        data_seq_tmp.back().discret = data_seq_tmp.back().to-data_seq_tmp.back().from;
                        data_seq_tmp.back().buf_pos_.push_back(data_seq.at(i).buf_pos_);
                    }
                }
            }
        }
    }
    returned.add_data(result);
    return returned;
}

void SublimedGribDataInfo::serialize(std::ofstream& file){
    if(!file.is_open())
        throw std::runtime_error("Not openned file-stream");
    using namespace translate::token;
    int token = (int)DataTypeInfo::Grib;
    file.write((const char*)&token,sizeof(int));
    int dont_exists = 0;
    for(const auto& [filename,filedata]:info_){
        if(!fs::exists(filename))
            ++dont_exists;
    }
    size_t data_sz = info_.size()-dont_exists;
    file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));        //number of files
    for(const auto& [filename,filedata]:info_){
        if(!fs::exists(filename))
            continue;
        data_sz = filename.size();                                              //length filename
        file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));    //filename
        file.write(filename.data(),data_sz);                                    //filename
        data_sz = filedata.size();                                              //number of grib variations
        file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));    //number of grib variations
        for(const auto& grib_info:filedata){
            //header
            file.write(reinterpret_cast<const char*>(grib_info.first.get()),sizeof(CommonDataProperties));
            data_sz = grib_info.second.size();
            //data-sequence size
            file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));
            //data
            for(const auto& sublimed_data:grib_info.second)
                sublimed_data.serialize(file);
        }
    }
    file.close();
}

void SublimedGribDataInfo::deserialize(std::ifstream& file){
    if(!file.is_open())
        throw std::runtime_error("Not openned file-stream");
    DataTypeInfo tokens_f;
    CommonDataProperties common;
    using namespace translate::token;
    while(file.read(reinterpret_cast<char*>(&tokens_f),sizeof(int))){
        switch ((tokens_f)){
        case DataTypeInfo::Grib:{
            size_t number_files = 0;
            SublimedGribDataInfo to_add;
            file.read((char*)(&number_files),sizeof(number_files));
            for(int i=0;i<number_files;++i){
                std::string filename;
                size_t length_filename = 0;
                file.read(reinterpret_cast<char*>(&length_filename),sizeof(length_filename));
                filename.resize(length_filename);
                file.read(filename.data(),length_filename);
                auto& file_data_tmp = to_add.info_[*to_add.paths_.emplace(filename).first];
                size_t grib_variations = 0;
                file.read(reinterpret_cast<char*>(&grib_variations),sizeof(grib_variations));
                for(int j=0;j<grib_variations;++j){
                    file.read(reinterpret_cast<char*>(&common),sizeof(CommonDataProperties));
                    auto& seq = file_data_tmp[std::make_shared<CommonDataProperties>(std::move(common))];
                    size_t data_sz=0;
                    //data-sequence size
                    file.read(reinterpret_cast<char*>(&data_sz),sizeof(data_sz));
                    seq.resize(data_sz);
                    //data
                    for(int m=0;m<data_sz;++m)
                        seq.at(m).deserialize(file);
                }
            }
            add_data(to_add);
            break;
        }
        default:
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'"s+bindata_filename.data()+type_to_extension(DataTypeInfo::Grib)+"\'",AT_ERROR_ACTION::ABORT);
            break;
        }
    }
}