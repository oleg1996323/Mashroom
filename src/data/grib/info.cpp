#include "data//info.h"

namespace fs = std::filesystem;
void GribDataInfo::add_info(const path::Storage<false>& path, const GribMsgDataInfo& msg_info) noexcept{
    info_[path][std::make_shared<CommonDataProperties>(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)]
    .emplace_back(GribCapitalizeDataInfo{
        msg_info.grid_data,
        msg_info.buf_pos_,
        msg_info.date,
        ErrorCodeData::NONE_ERR});
}

void GribDataInfo::add_info(const path::Storage<false>& path, GribMsgDataInfo&& msg_info) noexcept{
    info_[path][std::make_shared<CommonDataProperties>(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)]
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
    for(auto& [path,path_data]:info_){
        auto& data_t_tmp = result[path];
        for(auto& [cmn_d,data_seq]:path_data){
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

// void SublimedGribDataInfo::serialize(std::ofstream& file){
//     if(!file.is_open())
//         throw std::runtime_error("Not openned file-stream");
//     using namespace translate::token;
//     int token = (int)__Data__::FORMAT::GRIB;
//     file.write((const char*)&token,sizeof(int));
//     int dont_exists = 0;
//     for(const auto& [path,filedata]:info_){
//         if(path.type_!=path::TYPE::HOST || !fs::exists(path.path_))
//             ++dont_exists;
//     }
//     size_t data_sz = info_.size()-dont_exists;
//     file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));        //number of files
//     for(const auto& [path,filedata]:info_){
//         if(path.type_!=path::TYPE::HOST || !fs::exists(path.path_))
//             continue;
//         data_sz = path.path_.size();                                              //length filename
//         file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));    //filename
//         file.write(path.path_.data(),data_sz);                                    //filename
//         file.write(reinterpret_cast<const char*>(&path.type_),sizeof(path.type_));
//         data_sz = filedata.size();                                              //number of grib variations
//         file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));    //number of grib variations
//         for(const auto& grib_info:filedata){
//             //header
//             file.write(reinterpret_cast<const char*>(grib_info.first.get()),sizeof(CommonDataProperties));
//             data_sz = grib_info.second.size();
//             //data-sequence size
//             file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));
//             //data
//             for(const auto& sublimed_data:grib_info.second){
//                 /** @todo Make serialization directly without bufferisation */
//                 std::vector<char> buffer;
//                 buffer.reserve(serialization::serial_size(sublimed_data));
//                 serialization::serialize_native(sublimed_data,buffer);
//                 file.write(buffer.data(),buffer.size());
//             }
//         }
//     }
//     file.close();
// }

// void SublimedGribDataInfo::deserialize(std::ifstream& file){
//     if(!file.is_open())
//         throw std::runtime_error("Not openned file-stream");
//     __Data__::FORMAT tokens_f;
//     CommonDataProperties common;
//     using namespace translate::token;
//     while(file.read(reinterpret_cast<char*>(&tokens_f),sizeof(int))){
//         switch ((tokens_f)){
//         case __Data__::FORMAT::GRIB:{
//             size_t number_files = 0;
//             SublimedGribDataInfo to_add;
//             file.read((char*)(&number_files),sizeof(number_files));
//             for(int i=0;i<number_files;++i){
//                 std::string path;
//                 path::TYPE path_type=path::TYPE(0);
//                 size_t length_filename = 0;
//                 file.read(reinterpret_cast<char*>(&length_filename),sizeof(length_filename));
//                 path.resize(length_filename);
//                 file.read(path.data(),length_filename);
//                 file.read(reinterpret_cast<char*>(&path_type),sizeof(path_type));
//                 auto& path_data_tmp = to_add.info_[*to_add.paths_.emplace(path,path_type).first];
//                 size_t grib_variations = 0;
//                 file.read(reinterpret_cast<char*>(&grib_variations),sizeof(grib_variations));
//                 for(int j=0;j<grib_variations;++j){
//                     file.read(reinterpret_cast<char*>(&common),sizeof(CommonDataProperties));
//                     auto& seq = path_data_tmp[std::make_shared<CommonDataProperties>(std::move(common))];
//                     size_t data_sz=0;
//                     //data-sequence size
//                     file.read(reinterpret_cast<char*>(&data_sz),sizeof(data_sz));
//                     seq.resize(data_sz);
//                     //data
//                     using namespace serialization;
//                     for(int m=0;m<data_sz;++m)
//                         if(SerializationEC err = deserialize_from_file(seq.at(m),file);
//                             err!=SerializationEC::NONE)
//                             return ErrorCode::DESERIALIZATION_ERROR;
//                 }
//             }
//             add_data(to_add);
//             break;
//         }
//         default:
//             ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'"s+bindata_filename.data()+type_to_extension(__Data__::FORMAT::GRIB)+"\'",AT_ERROR_ACTION::ABORT);
//             break;
//         }
//     }
// }
// #include <cstring>
// void SublimedGribDataInfo::serialize(std::vector<char>& buf) const{
//     using namespace translate::token;
//     using namespace serialization;
//     int token = (int)__Data__::FORMAT::GRIB;
    
//     buf.insert(buf.end(),(const char*)&token,(const char*)&token+sizeof(token));
//     int dont_exists = 0;
//     for(const auto& [path,filedata]:info_){
//         if(path.type_!=path::TYPE::HOST || !fs::exists(path.path_))
//             ++dont_exists;
//     }
//     size_t data_sz = info_.size()-dont_exists;
//     buf.insert(buf.end(),(const char*)&data_sz,(const char*)&data_sz+sizeof(data_sz));
//     for(const auto& [path,filedata]:info_){
//         if(path.type_!=path::TYPE::HOST){
//             if(!fs::exists(path.path_))
//                 continue;
//             else{
//                 buf.insert(buf.end(),(const char*)&path.type_,(const char*)&path.type_+sizeof(path.type_));
//             }
//         }
//         else{
//             buf.insert(buf.end(),(const char*)&path.type_,(const char*)&path.type_+sizeof(path.type_));
//             size_t path_sz = path.path_.size();
//             buf.insert(buf.end(),(const char*)&path_sz,(const char*)&path_sz+sizeof(path_sz));
//             buf.insert(buf.end(),path.path_.data(),path.path_.data()+path_sz);
//         }
//         data_sz = filedata.size();                                              //number of grib variations
//         buf.insert(buf.end(),(const char*)&data_sz,(const char*)&data_sz+sizeof(data_sz));
//         for(const auto& grib_info:filedata){
//             //header
//             buf.insert(buf.end(),(const char*)grib_info.first.get(),(const char*)grib_info.first.get()+sizeof(CommonDataProperties));
//             data_sz = grib_info.second.size();
//             //data-sequence size
//             buf.insert(buf.end(),(const char*)&data_sz,(const char*)&data_sz+sizeof(data_sz));
//             //data
//             for(const auto& sublimed_data:grib_info.second)
//                 serialization::serialize_native(sublimed_data, buf);
//         }
//     }
// }
// void SublimedGribDataInfo::deserialize(std::vector<char>& buf) const{
//     __Data__::FORMAT tokens_f;
//     size_t read = 0;
//     using namespace translate::token;
//     while(read<buf.size()){
//         switch ((tokens_f)){
//         case __Data__::FORMAT::GRIB:{
//             size_t number_blocks = *(size_t*)(buf.data()+read);
//             SublimedGribDataInfo to_add;
//             read+=sizeof(number_blocks);
//             for(int i=0;i<number_blocks;++i){
//                 path::TYPE path_type = *(path::TYPE*)(buf.data()+read);
//                 read+=sizeof(path_type);
//                 std::string_view path;
//                 if(path_type==path::TYPE::HOST){
//                     size_t path_sz = *(size_t*)(buf.data()+read);
//                     assert(path_sz>0);
//                     read+=sizeof(path_sz);
//                     path = std::string_view(buf.data()+read,path_sz);
//                     read+=path_sz;
//                 }
//                 auto& path_data_tmp = to_add.info_[*to_add.paths_.emplace(path,path_type).first]; //if not host, then empty string
//                 size_t grib_variations = *(size_t*)(buf.data()+read);
//                 read+=sizeof(grib_variations);
//                 for(int j=0;j<grib_variations;++j){
//                     std::vector<SublimedDataInfo>& seq = path_data_tmp[std::make_shared<CommonDataProperties>(*(CommonDataProperties*)(buf.data()+read))];
//                     read+=sizeof(CommonDataProperties);
//                     //data-sequence size
//                     seq.resize(*(size_t*)(buf.data()+read));
//                     read+=sizeof(size_t);
//                     //data
//                     for(auto& sub_data:seq)
//                         sub_data.deserialize(buf,read);
//                 }
//             }
//             break;
//         }
//         default:
//             ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'"s+bindata_filename.data()+type_to_extension(__Data__::FORMAT::GRIB)+"\'",AT_ERROR_ACTION::ABORT);
//             break;
//         }
//     }
// }