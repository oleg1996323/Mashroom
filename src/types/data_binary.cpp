#include "types/data_binary.h"

namespace fs = std::filesystem;
using namespace std::string_literals;

BinaryGribInfo BinaryGribInfo::read(const fs::path& path){
    std::ifstream file(path.c_str(),std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,path.string());
    BinaryGribInfo format;
    int count = 0;
    std::vector<char> str;
    str.resize(BUFSIZ);
    using namespace translate::token;
    while(file.read(str.data(),sizeof(int))){
        switch ((FormatTokens)(*(int*)(str.data()))){
        case FormatTokens::Capitalize_format:{
            size_t order_sz = 0;
            file.read((char*)(&order_sz),sizeof(order_sz));
            format.order_.resize(order_sz);
            file.read(format.order_.data(),order_sz);
            break;
        }
        case FormatTokens::Capitalize_data:{
            size_t number_elements = 0;
            file.read((char*)(&number_elements),sizeof(number_elements));
            for(int i=0;i<number_elements;++i){
                file.read(str.data(),sizeof(CommonDataProperties));
                size_t data_sz=0;
                file.read((char*)(&data_sz),sizeof(data_sz));
                GribDataInfo::data_t::mapped_type data_read;
                file.read((char*)(&data_read),sizeof(GribCapitalizeDataInfo)*data_sz);
                format.data_.add_info(*(CommonDataProperties*)str.data(),std::move(data_read));
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
void BinaryGribInfo::write(const fs::path& dir,BinaryGribInfoType bin_info_type){
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
    size_t data_sz = data_.data().size();
    file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));
    for(auto& grib_info:data_.data()){
        //header
        file.write(reinterpret_cast<const char*>(&grib_info.first),sizeof(CommonDataProperties));
        data_sz = grib_info.second.size();
        //data-sequence size
        file.write(reinterpret_cast<const char*>(&data_sz),sizeof(data_sz));
        //data
        file.write(reinterpret_cast<const char*>(&grib_info.second),sizeof(GribCapitalizeDataInfo)*data_sz);
    }
    file.close();
}