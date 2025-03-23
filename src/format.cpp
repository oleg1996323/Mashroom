#include "format.h"

namespace fs = std::filesystem;

namespace format{

using namespace std::string_literals;

FormatBinData read(const fs::path& path){
    std::ifstream file(path.c_str(),std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,path.string());
    FormatBinData format;
    int count = 0;
    char str[BUFSIZ];
    using namespace translate::token;
    while(file.read(str,sizeof(int))){
        int to_read = *(int*)(str);
        if(!file.read(str,sizeof(int)))
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'"s+format_filename+"\'",AT_ERROR_ACTION::ABORT);
        FormatTokens token = (FormatTokens)(*(int*)(str));
        switch (token){
        case FormatTokens::Capitalize_format:
            file.read(str,to_read);
            format.order_ = *reinterpret_cast<OrderItems*>(str);
            break;
        case FormatTokens::Capitalize_data:
            file.read(str,to_read);
            format.data_ = *reinterpret_cast<CapitalizeData*>(str);
            break;
        default:
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'"s+format_filename+"\'",AT_ERROR_ACTION::ABORT);
            break;
        }
    }
    if(!is_correct_date(&format.data_.from))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"Corrupted file \'"s+format_filename+"\'",AT_ERROR_ACTION::ABORT);
    if(!is_correct_date(&format.data_.to))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"Corrupted file \'"s+format_filename+"\'",AT_ERROR_ACTION::ABORT);
    if(!is_correct_rect(&format.data_.grid_data.bound))
        ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Corrupted file \'"s+format_filename+"\'",AT_ERROR_ACTION::ABORT);
    return format;
}

void write(const fs::path& dir,const OrderItems& order,const CapitalizeData& data){
    if(!fs::is_directory(dir))
        ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    std::ofstream file(dir/format_filename,std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dir/format_filename).c_str());
    using namespace translate::token;
    int size = sizeof(OrderItems);
    int token = (int)FormatTokens::Capitalize_format;
    file.write((const char*)&size,sizeof(int)).write((const char*)&token,sizeof(int)).write((const char*)(&order),sizeof(OrderItems));
    size = sizeof(CapitalizeData);
    token = (int)FormatTokens::Capitalize_data;
    file.write((const char*)&size,sizeof(int)).write((const char*)&token,sizeof(int)).write((const char*)(&data),sizeof(CapitalizeData));
    file.close();
}

void write(const fs::path& dir,const FormatBinData& data){
    if(!fs::is_directory(dir))
        ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    std::ofstream file(dir/format_filename,std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dir/format_filename).c_str());
    using namespace translate::token;
    int size = sizeof(OrderItems);
    int token = (int)FormatTokens::Capitalize_format;
    file.write((const char*)&size,sizeof(int)).write((const char*)&token,sizeof(int)).write((const char*)(&data.order_),sizeof(OrderItems));
    size = sizeof(CapitalizeData);
    token = (int)FormatTokens::Capitalize_data;
    file.write((const char*)&size,sizeof(int)).write((const char*)&token,sizeof(int)).write((const char*)(&data.data_),sizeof(CapitalizeData));
    file.close();
}
}