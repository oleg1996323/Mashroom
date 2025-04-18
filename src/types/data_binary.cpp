#include "types/data_binary.h"
#include "cmd_parse/cmd_translator.h"

namespace fs = std::filesystem;
using namespace std::string_literals;

BinaryGribInfo BinaryGribInfo::read(const fs::path& path){
    std::ifstream file(path.c_str(),std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,path.string());
    BinaryGribInfo format;
    int count = 0;
    FormatTokens tokens_f;
    CommonDataProperties common;
    using namespace translate::token;
    while(file.read(reinterpret_cast<char*>(&tokens_f),sizeof(int))){
        switch ((tokens_f)){
        case FormatTokens::Capitalize_format:{
            size_t order_sz = 0;
            file.read((char*)(&order_sz),sizeof(order_sz));
            format.order_.resize(order_sz);
            file.read(format.order_.data(),order_sz);
            break;
        }
        case FormatTokens::Capitalize_data:{
            size_t number_files = 0;
            file.read((char*)(&number_files),sizeof(number_files));
            for(int i=0;i<number_files;++i){
                std::string filename;
                size_t length_filename = 0;
                file.read(reinterpret_cast<char*>(&length_filename),sizeof(length_filename));
                filename.resize(length_filename);
                size_t grib_variations = 0;
                file.read(reinterpret_cast<char*>(&grib_variations),sizeof(grib_variations));
                for(int j=0;j<grib_variations;++j){
                    file.read(reinterpret_cast<char*>(&common),sizeof(CommonDataProperties));
                    size_t data_sz=0;
                    std::vector<SublimedGribCapitalizeDataInfo> sublimed;
                    //data-sequence size
                    file.read(reinterpret_cast<char*>(&data_sz),sizeof(data_sz));
                    sublimed.resize(data_sz);
                    //data
                    for(int m=0;m<data_sz;++m){
                        sublimed[m].deserialize(file);
                        format.add_info(filename,common,std::move(sublimed[m].desublime()));
                    }
                }
            }
            break;
        }
        default:
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'"s+format_filename+"\'",AT_ERROR_ACTION::ABORT);
            break;
        }
    }
    return format;
}
#include "cmd_parse/cmd_translator.h"
void BinaryGribInfo::write(const fs::path& dir){
    sublimed_data_t sublimed = sublime();
    if(!fs::is_directory(dir))
        ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    std::ofstream file(dir/format_filename,std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dir/format_filename).c_str());
    using namespace translate::token;
    int token = (int)FormatTokens::Capitalize_format;
    size_t order_sz = order_.size();
    file.write((const char*)&token,sizeof(int)).write(reinterpret_cast<const char*>(&order_sz),sizeof(order_sz)).write(order_.c_str(),1);
    token = (int)FormatTokens::Capitalize_data;
    file.write((const char*)&token,sizeof(int));
    size_t data_sz = sublimed.size();
    file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));        //number of files
    for(const auto& [filename,filedata]:sublimed){
        data_sz = filename.string().size();                                     //length filename
        file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));    //filename
        file.write(filename.c_str(),sizeof(data_sz));                           //filename
        data_sz = filedata.size();                                              //number of grib variations
        file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));    //number of grib variations
        for(const auto& grib_info:filedata){
            //header
            file.write(reinterpret_cast<const char*>(&grib_info.first),sizeof(CommonDataProperties));
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