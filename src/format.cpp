#include "format.h"

namespace fs = std::filesystem;

namespace format{

FormatData read(const fs::path& path){
    std::ifstream file(path,std::ios::binary);
    FormatData format;
    Rect rect = Rect();
    DataExtractMode extract_mode = DataExtractMode::UNDEF;
    int count = 0;
    char* str;
    using namespace translate::token;
    while(file.read(str,sizeof(int))){
        int to_read = std::stoi(str);
        if(!file.read(str,sizeof(int)))
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
        Command token = (Command)std::stoi(str);
        switch (token){
        case Command::CAPITALIZE_HIERARCHY:
            file.read(str,to_read);
            format.order = *reinterpret_cast<OrderItems*>(str);
            break;
        case Command::DATE_FROM:
        //Date get_date_from_token(std::string_view token)
            file.read(str,to_read);
            format.from = *reinterpret_cast<Date*>(str);
            break;
        case Command::DATE_TO:
            file.read(str,to_read);
            format.to = *reinterpret_cast<Date*>(str);
            break;
        case Command::POSITION:{
            file.read(str,to_read);
            format.coord_data = *reinterpret_cast<Coord*>(str);
            break;
        }
        case Command::LAT_TOP:
            file.read(str,to_read);
            format.coord_data = *reinterpret_cast<Rect*>(str);
            break;
        default:
            ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
            break;
        }
    }
    if(extract_mode==DataExtractMode::RECT && std::holds_alternative<std::monostate>(format.coord_data))
        format.coord_data = rect;
    else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    if(!is_correct_date(&format.from))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    if(!is_correct_date(&format.to))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    if(extract_mode==DataExtractMode::RECT && (std::holds_alternative<Rect>(format.coord_data) || !is_correct_rect
    (&rect)))
        ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    else if(extract_mode==DataExtractMode::POSITION && (std::holds_alternative<Coord>(format.coord_data) || !is_correct_pos(&std::get<Coord>(format.coord_data))))
        ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Corrupted file \'format.bin\'",AT_ERROR_ACTION::ABORT);
    return format;
}

void write(const fs::path& dir,OrderItems order,const Date& from, const Date& to, Coord pos){
    if(!fs::is_directory(dir))
        ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    std::ofstream file(dir/"format.bin",std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dir/"format.bin").c_str());
    using namespace translate::token;
    (file<<sizeof(OrderItems)<<(int)Command::CAPITALIZE_HIERARCHY).write(reinterpret_cast<const char*>(&order),sizeof(OrderItems));
    (file<<sizeof(Date)<<(int)Command::DATE_FROM).write(reinterpret_cast<const char*>(&from),sizeof(Date));
    (file<<sizeof(Date)<<(int)Command::DATE_TO).write(reinterpret_cast<const char*>(&to),sizeof(Date));
    (file<<sizeof(Coord)<<(int)Command::POSITION).write(reinterpret_cast<const char*>(&pos),sizeof(Coord));
    file.close();
}

void write(const fs::path& dir,OrderItems order,const Date& from, const Date& to, const Rect& rect){
    if(!fs::is_directory(dir))
        ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::ABORT,dir.c_str());
    std::ofstream file(dir/"format.bin",std::ios::binary);
    if(!file.is_open())
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dir/"format.bin").c_str());
    using namespace translate::token;
    (file<<sizeof(OrderItems)<<(int)Command::CAPITALIZE_HIERARCHY).write(reinterpret_cast<const char*>(&order),sizeof(OrderItems));
    (file<<sizeof(Date)<<(int)Command::DATE_FROM).write(reinterpret_cast<const char*>(&from),sizeof(Date));
    (file<<sizeof(Date)<<(int)Command::DATE_TO).write(reinterpret_cast<const char*>(&to),sizeof(Date));
    (file<<sizeof(Rect)<<(int)Command::LAT_TOP).write(reinterpret_cast<const char*>(&rect),sizeof(Rect));
    file.close();
}
}