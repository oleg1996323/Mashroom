#include "extract_parse.h"
#include <thread>
#include "extract_cpp.h"
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "functions.h"
#include "format.h"

namespace fs = std::filesystem;

cpp::DATA_OUT get_extract_format(std::string_view input){
    std::vector<std::string_view> tokens = split(input,":");
    if(tokens.empty())
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid input at extract format definition",AT_ERROR_ACTION::ABORT,input);
    else{
        cpp::DATA_OUT result;
        result = cpp::DATA_OUT::UNDEF;
        for(std::string_view token:tokens){
            if(token.empty())
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid input at extract format definition",AT_ERROR_ACTION::ABORT,input);
            else
                switch (translate_from_txt<translate::token::ExtractFormatArgs>(token))
                {
                case translate::token::ExtractFormatArgs::ARCHIVED:{
                    if(result&cpp::DATA_OUT::ARCHIVED)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Archive format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        result|=cpp::DATA_OUT::ARCHIVED;
                    break;
                }
                case translate::token::ExtractFormatArgs::BIN:{
                    cpp::DATA_OUT cur = result^cpp::DATA_OUT::ARCHIVED;
                    if(cur&(cpp::DATA_OUT::DEFAULT|cpp::DATA_OUT::UNDEF))
                        result|=cpp::DATA_OUT::BIN_F;
                    else if(cur==cpp::DATA_OUT::BIN_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Binary format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                    break;
                }
                case translate::token::ExtractFormatArgs::GRIB:{
                    cpp::DATA_OUT cur = result^cpp::DATA_OUT::ARCHIVED;
                    if(cur&(cpp::DATA_OUT::DEFAULT|cpp::DATA_OUT::UNDEF))
                        result|=cpp::DATA_OUT::GRIB_F;
                    else if(cur==cpp::DATA_OUT::GRIB_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Grib format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                    break;
                }
                case translate::token::ExtractFormatArgs::TXT:{
                    cpp::DATA_OUT cur = result|cpp::DATA_OUT::ARCHIVED^cpp::DATA_OUT::ARCHIVED;
                    if(cur&(cpp::DATA_OUT::DEFAULT|cpp::DATA_OUT::UNDEF))
                        result|=cpp::DATA_OUT::TXT_F;
                    else if(cur==cpp::DATA_OUT::TXT_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Text format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                    break;
                }
                default:
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for extract mode",AT_ERROR_ACTION::ABORT,token);
                    return (cpp::DATA_OUT)-1;
                }
        }
        return result;
    }
    return (cpp::DATA_OUT)-1;
}

void extract_parse(const std::vector<std::string_view>& input){
    Date date_from = Date();
    Date date_to = Date();
    std::filesystem::path in;
    std::filesystem::path out;
    DataExtractMode mode_extract = DataExtractMode::UNDEF;
    cpp::DATA_OUT extract_out_fmt;
    cpp::DIV_DATA_OUT extract_div_data;
    Rect rect = Rect();
    Coord coord = Coord();
    unsigned int cpus = std::thread::hardware_concurrency();

    for(size_t i=0;i<input.size();++i){
        switch (translate_from_txt<translate::token::Command>(input[i++]))
        {
            case translate::token::Command::THREADS:{
                long tmp = from_chars<long>(input[i]);
                if(tmp>=1 & tmp<=std::thread::hardware_concurrency())
                    cpus = tmp;
                break;
            }
            case translate::token::Command::IN_PATH:{
                in = input[i];
                if(!fs::is_directory(in))
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,in.c_str());
                if(!fs::exists(in/format_filename))
                    ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::ABORT,(in/format_filename).c_str());
                break;
            }
            case translate::token::Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2)
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::ABORT,input[i]);
                if(tokens.at(0)=="dir"){
                    out = std::string(tokens.at(1));
                    if(!fs::exists(out)){
                        if(!fs::create_directory(out))
                            ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::ABORT,out.c_str());
                    }
                    else{
                        if(!fs::is_directory(out))
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,out.c_str());
                    }
                }
                else if(tokens.at(0)=="ip")
                //TODO
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use capitalize mode by IP",AT_ERROR_ACTION::ABORT,tokens[0]);
                else
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for extract mode",AT_ERROR_ACTION::ABORT,input[i]);
                break;
            }
            case translate::token::Command::DATE_FROM:{
                date_from = get_date_from_token(input[i]);
                break;
            }
            case translate::token::Command::DATE_TO:{
                date_to = get_date_from_token(input[i]);
                break;
            }
            case translate::token::Command::LAT_TOP:{
                try{
                    rect.y1 = get_coord_from_token<double>(input[i],mode_extract);
                }
                catch(const std::invalid_argument& err){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
                }
                break;
            }
            case translate::token::Command::LAT_BOT:{
                try{
                    rect.y2 = get_coord_from_token<double>(input[i],mode_extract);
                }
                catch(const std::invalid_argument& err){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
                }
                break;
            }
            case translate::token::Command::LON_LEFT:{
                try{
                    rect.x1 = get_coord_from_token<double>(input[i],mode_extract);
                }
                catch(const std::invalid_argument& err){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
                }
                break;
            }
            case translate::token::Command::LON_RIG:{
                try{
                    rect.x2 = get_coord_from_token<double>(input[i],mode_extract);
                }
                catch(const std::invalid_argument& err){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
                }
                break;
            }
            case translate::token::Command::EXTRACT_FORMAT:{
                extract_out_fmt = get_extract_format(input[i]);
                break;
            }
            case translate::token::Command::EXTRACTION_DIV:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Extract division data format still not supported in this Mashroom's version",AT_ERROR_ACTION::ABORT,input[i]);
                /*
                ++i;
                if(input[i]=="h")
                    extract_div_data = cpp::DIV_DATA_OUT::HOUR_T;
                else if(input[i]=="y")
                    extract_div_data = cpp::DIV_DATA_OUT::YEAR_T;
                else if(input[i]=="m")
                    extract_div_data = cpp::DIV_DATA_OUT::MONTH_T;
                else if(input[i]=="d")
                    extract_div_data = cpp::DIV_DATA_OUT::DAY_T;
                else if(input[i]=="lat")
                    extract_div_data = cpp::DIV_DATA_OUT::LAT;
                else if(input[i]=="lon")
                    extract_div_data = cpp::DIV_DATA_OUT::LON;
                else if(input[i]=="latlon")
                    extract_div_data = (cpp::DIV_DATA_OUT)(cpp::DIV_DATA_OUT::LAT|cpp::DIV_DATA_OUT::LON);
                else{
                    std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
                    exit(1);
                }
                */
            }
            case translate::token::Command::POSITION:{
                coord = get_coord_from_token<Coord>(input[i],mode_extract);    
                break;        
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::ABORT,input[i]);
                break;
            }
        }
    }
    if(!fs::exists(in))
        ErrorPrint::print_error(ErrorCode::INVALID_PATH_OF_DIRECTORIES_X1,"",AT_ERROR_ACTION::ABORT,in.c_str());

    if(!is_correct_date(&date_from))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"Date from which make extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    if(!is_correct_date(&date_to))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"Date to which make extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    
    if(mode_extract==DataExtractMode::POSITION){
        if(!is_correct_pos(&coord)) //actually for WGS84
            ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
        cpp::extract_cpp_pos(in,out,date_from,date_to,coord,extract_out_fmt);
    }
    else if(mode_extract==DataExtractMode::RECT){
        if(!correct_rect(&rect))
            ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
        cpp::extract_cpp_rect(in,date_from,date_to,rect,extract_out_fmt);
    }
    else
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
}

std::vector<std::string_view> commands_from_extract_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands;
    commands.push_back(translate_from_token(translate::token::ModeArgs::EXTRACT));
    Date date_from = Date();
    Date date_to = Date();
    DataExtractMode mode_extract = DataExtractMode::UNDEF;
    cpp::DATA_OUT extract_out_fmt;
    cpp::DIV_DATA_OUT extract_div_data;
    Rect rect = Rect();
    Coord coord = Coord();
    unsigned int cpus = std::thread::hardware_concurrency();

    for(size_t i=0;i<input.size();++i){
        switch (translate_from_txt<translate::token::Command>(commands.emplace_back(input[i++])))
        {
            case translate::token::Command::THREADS:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::IN_PATH:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                if(!fs::is_directory(input.at(i)))
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,input.at(i));
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::OUT_PATH:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                std::vector<std::string_view> tokens = split(input.at(i),":");
                if(tokens.size()!=2)
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::ABORT,input.at(i));
                if(tokens.at(0)=="dir"){
                    if(!fs::is_directory(tokens.at(1))){
                        if(!fs::create_directory(tokens.at(1)))
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,tokens.at(1));
                    }
                    commands.push_back(input.at(i));
                }
                else if(tokens.at(0)=="ip")
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use check mode by IP",AT_ERROR_ACTION::ABORT,tokens[0]);
                else
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for check mode",AT_ERROR_ACTION::ABORT,input.at(i));
                break;
            }
            case translate::token::Command::DATE_FROM:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                date_from = get_date_from_token(input.at(i));
                if(is_correct_date(&date_from))
                    commands.push_back(input.at(i));
                else
                    ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::ABORT,input.at(i));
                break;
            }
            case translate::token::Command::DATE_TO:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                date_to = get_date_from_token(input.at(i));
                if(is_correct_date(&date_to))
                    commands.push_back(input.at(i));
                else
                    ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::ABORT,input.at(i));
                break;
            }
            case translate::token::Command::LAT_TOP:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                try{
                    rect.y1 = get_coord_from_token<double>(input[i],mode_extract);
                    commands.push_back(input.at(i));
                }
                catch(const std::invalid_argument& err){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
                }
                break;
            }
            case translate::token::Command::LAT_BOT:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                try{
                    rect.y2 = get_coord_from_token<double>(input[i],mode_extract);
                    commands.push_back(input.at(i));
                }
                catch(const std::invalid_argument& err){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
                }
                break;
            }
            case translate::token::Command::LON_LEFT:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                try{
                    rect.x1 = get_coord_from_token<double>(input[i],mode_extract);
                    commands.push_back(input.at(i));
                }
                catch(const std::invalid_argument& err){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
                }
                break;
            }
            case translate::token::Command::LON_RIG:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                try{
                    rect.x2 = get_coord_from_token<double>(input[i],mode_extract);
                    commands.push_back(input.at(i));
                }
                catch(const std::invalid_argument& err){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
                }
                break;
            }
            case translate::token::Command::EXTRACT_FORMAT:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                extract_out_fmt = get_extract_format(input[i]);
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::EXTRACTION_DIV:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Extract division data format still not supported in this Mashroom's version",AT_ERROR_ACTION::ABORT,input[i]);
                /*
                ++i;
                if(input[i]=="h")
                    extract_div_data = cpp::DIV_DATA_OUT::HOUR_T;
                else if(input[i]=="y")
                    extract_div_data = cpp::DIV_DATA_OUT::YEAR_T;
                else if(input[i]=="m")
                    extract_div_data = cpp::DIV_DATA_OUT::MONTH_T;
                else if(input[i]=="d")
                    extract_div_data = cpp::DIV_DATA_OUT::DAY_T;
                else if(input[i]=="lat")
                    extract_div_data = cpp::DIV_DATA_OUT::LAT;
                else if(input[i]=="lon")
                    extract_div_data = cpp::DIV_DATA_OUT::LON;
                else if(input[i]=="latlon")
                    extract_div_data = (cpp::DIV_DATA_OUT)(cpp::DIV_DATA_OUT::LAT|cpp::DIV_DATA_OUT::LON);
                else{
                    std::cout<<"Unknown token for capitalize mode hierarchy. Abort"<<std::endl;
                    exit(1);
                }
                */
            }
            // case translate::token::Command::CAPITALIZE_FORMAT:{
            //     //define after in-path defined and read format.bin
            //      format (grib,archive,txt,bin)(hierarchy)(begin date)(end date)(lattop,latbot,lonleft,lonrig)
            // }
                break;
            case translate::token::Command::POSITION:{
                coord = get_coord_from_token<Coord>(input[i],mode_extract);
                commands.push_back(input.at(i));
                break;        
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::ABORT,input[i]);
                break;
            }
        }
    }
    if(!is_correct_date(&date_from))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"Date from which make extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    if(!is_correct_date(&date_to))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"Date to which make extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    
    if(mode_extract==DataExtractMode::POSITION){
        if(!is_correct_pos(&coord)) //actually for WGS84
            ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
        return commands;
    }
    else if(mode_extract==DataExtractMode::RECT){
        if(!correct_rect(&rect))
            ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
        return commands;
    }
    else
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    return commands;
}