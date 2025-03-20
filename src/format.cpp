#include "format.h"

namespace fs = std::filesystem;

namespace format{

FormatBinData read(const fs::path& path){
    std::ifstream file(path,std::ios::binary);
    FormatBinData format;
    int count = 0;
    char* str;
    using namespace translate::token;
    while(file.read(str,sizeof(int))){
        int to_read = std::stoi(str);
        if(!file.read(str,sizeof(int)))
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
        FormatTokens token = (FormatTokens)std::stoi(str);
        switch (token){
        case FormatTokens::Capitalize_format:
            file.read(str,to_read);
            format.order_ = *reinterpret_cast<OrderItems*>(str);
            break;
        case FormatTokens::Capitalize_data:
            file.read(str,to_read);
            format.data_ = *reinterpret_cast<CapitalizeData*>(str);
            break;
        // case FormatTokens::DateFrom:
        // //Date get_date_from_token(std::string_view token)
        //     file.read(str,to_read);
        //     format.from_ = *reinterpret_cast<Date*>(str);
        //     break;
        // case Command::DATE_TO:
        //     file.read(str,to_read);
        //     format.to = *reinterpret_cast<Date*>(str);
        //     break;
        // case Command::POSITION:{
        //     file.read(str,to_read);
        //     format.coord_data = *reinterpret_cast<Coord*>(str);
        //     break;
        // }
        // case Command::LAT_TOP:
        //     file.read(str,to_read);
        //     format.coord_data = *reinterpret_cast<Rect*>(str);
        //     break;
        default:
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
            break;
        }
    }
    if(!is_correct_date(&format.data_.from))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    if(!is_correct_date(&format.data_.to))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    if(!is_correct_rect(&format.data_.grid_data.bound))
        ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    return format;
}

// void write(const fs::path& dir,OrderItems order,const Date& from, const Date& to, Coord pos){
//     if(!fs::is_directory(dir))
//         ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
//     std::ofstream file(dir/"format.bin",std::ios::binary);
//     if(!file.is_open())
//         ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dir/"format.bin").c_str());
//     using namespace translate::token;
//     (file<<sizeof(OrderItems)<<(int)Command::CAPITALIZE_HIERARCHY).write(reinterpret_cast<const char*>(&order),sizeof(OrderItems));
//     (file<<sizeof(Date)<<(int)Command::DATE_FROM).write(reinterpret_cast<const char*>(&from),sizeof(Date));
//     (file<<sizeof(Date)<<(int)Command::DATE_TO).write(reinterpret_cast<const char*>(&to),sizeof(Date));
//     (file<<sizeof(Coord)<<(int)Command::POSITION).write(reinterpret_cast<const char*>(&pos),sizeof(Coord));
//     file.close();
// }

void write(const fs::path& dir,const OrderItems& order,const CapitalizeData& data){
    if(!fs::is_directory(dir))
        ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    std::ofstream file(dir/"format.bin",std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dir/"format.bin").c_str());
    using namespace translate::token;
    (file<<sizeof(OrderItems)<<(int)FormatTokens::Capitalize_format).write(reinterpret_cast<const char*>(&order),sizeof(OrderItems));
    (file<<sizeof(CapitalizeData)<<(int)FormatTokens::Capitalize_data).write(reinterpret_cast<const char*>(&data),sizeof(CapitalizeData));
    file.close();
}

void write(const fs::path& dir,const FormatBinData& data){
    if(!fs::is_directory(dir))
        ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    std::ofstream file(dir/"format.bin",std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dir/"format.bin").c_str());
    using namespace translate::token;
    (file<<sizeof(OrderItems)<<(int)FormatTokens::Capitalize_format).write(reinterpret_cast<const char*>(&data.order_),sizeof(OrderItems));
    (file<<sizeof(CapitalizeData)<<(int)FormatTokens::Capitalize_data).write(reinterpret_cast<const char*>(&data.data_),sizeof(CapitalizeData));
    file.close();
}
}