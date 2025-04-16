#include "extract_parse.h"
#include <thread>
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "functions.h"
#include "types/data_binary.h"
#include "extract.h"

namespace fs = std::filesystem;

Extract::ExtractFormat get_extract_format(std::string_view input){
    Extract::ExtractFormat result;
    std::vector<std::string_view> tokens = split(input,":");
    if(tokens.empty())
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid input at extract format definition",AT_ERROR_ACTION::ABORT,input);
    else{
        for(std::string_view token:tokens){
            if(token.empty())
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid input at extract format definition",AT_ERROR_ACTION::ABORT,input);
            else
                switch (translate_from_txt<translate::token::FileFormat>(token))
                {
                case translate::token::FileFormat::ARCHIVED:{
                    if((result&Extract::ExtractFormat::ARCHIVED)!=(Extract::ExtractFormat)0)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Archive format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        result|=Extract::ExtractFormat::ARCHIVED;
                    break;
                }
                case translate::token::FileFormat::BIN:{
                    Extract::ExtractFormat cur = result^Extract::ExtractFormat::ARCHIVED;
                    if(cur==Extract::ExtractFormat::DEFAULT)
                        result|=Extract::ExtractFormat::BIN_F;
                    else if(cur==Extract::ExtractFormat::BIN_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Binary format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                    break;
                }
                case translate::token::FileFormat::GRIB:{
                    Extract::ExtractFormat cur = result^Extract::ExtractFormat::ARCHIVED;
                    if(cur==Extract::ExtractFormat::DEFAULT)
                        result|=Extract::ExtractFormat::GRIB_F;
                    else if(cur==Extract::ExtractFormat::GRIB_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Grib format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                    break;
                }
                case translate::token::FileFormat::TXT:{
                    Extract::ExtractFormat cur = result|Extract::ExtractFormat::ARCHIVED^Extract::ExtractFormat::ARCHIVED;
                    if(cur==Extract::ExtractFormat::DEFAULT)
                        result|=Extract::ExtractFormat::TXT_F;
                    else if(cur==Extract::ExtractFormat::TXT_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Text format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                    break;
                }
                default:
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for extract mode",AT_ERROR_ACTION::ABORT,token);
                    return (Extract::ExtractFormat)-1;
                }
        }
        return result;
    }
    return (Extract::ExtractFormat)-1;
}

void extract_parse(const std::vector<std::string_view>& input){
    Extract hExtract;
    std::chrono::system_clock::time_point date_from;
    std::chrono::system_clock::time_point date_to;
    std::filesystem::path in;
    std::filesystem::path out;
    DataExtractMode mode_extract = DataExtractMode::UNDEF;
    Extract::ExtractFormat extract_out_fmt;
    
    Coord coord = Coord();
    unsigned int cpus = std::thread::hardware_concurrency();

    for(size_t i=0;i<input.size();++i){
        switch (translate_from_txt<translate::token::Command>(input[i++]))
        {
            case translate::token::Command::THREADS:
                hExtract.set_using_processor_cores(from_chars<long>(input[i]));
                break;
            case translate::token::Command::IN_PATH:
                hExtract.set_from_path(input[i]);
                break;
            case translate::token::Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2)
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::ABORT,input[i]);
                if(tokens.at(0)=="dir")
                    hExtract.set_dest_dir(tokens.at(1));
                else if(tokens.at(0)=="ip")
                //TODO
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use extract mode by IP",AT_ERROR_ACTION::ABORT,tokens[0]);
                else
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for extract mode",AT_ERROR_ACTION::ABORT,input[i]);
                break;
            }
            case translate::token::Command::DATE_FROM:{
                hExtract.set_from_date(get_date_from_token(input[i]));
                break;
            }
            case translate::token::Command::DATE_TO:{
                hExtract.set_to_date(get_date_from_token(input[i]));
                break;
            }
            // case translate::token::Command::LAT_TOP:{
            //     try{
            //         rect.y1 = get_coord_from_token<double>(input[i],mode_extract);
            //     }
            //     catch(const std::invalid_argument& err){
            //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
            //     }
            //     break;
            // }
            // case translate::token::Command::LAT_BOT:{
            //     try{
            //         rect.y2 = get_coord_from_token<double>(input[i],mode_extract);
            //     }
            //     catch(const std::invalid_argument& err){
            //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
            //     }
            //     break;
            // }
            // case translate::token::Command::LON_LEFT:{
            //     try{
            //         rect.x1 = get_coord_from_token<double>(input[i],mode_extract);
            //     }
            //     catch(const std::invalid_argument& err){
            //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
            //     }
            //     break;
            // }
            // case translate::token::Command::LON_RIG:{
            //     try{
            //         rect.x2 = get_coord_from_token<double>(input[i],mode_extract);
            //     }
            //     catch(const std::invalid_argument& err){
            //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
            //     }
            //     break;
            // }
            case translate::token::Command::EXTRACT_FORMAT:{
                hExtract.set_output_format(get_extract_format(input[i]));
                break;
            }
            case translate::token::Command::EXTRACTION_DIV:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Extract division data format still not supported in this Mashroom's version",AT_ERROR_ACTION::ABORT,input[i]);
                /*
                ++i;
                if(input[i]=="h")
                    extract_div_data = DIV_DATA_OUT::HOUR_T;
                else if(input[i]=="y")
                    extract_div_data = DIV_DATA_OUT::YEAR_T;
                else if(input[i]=="m")
                    extract_div_data = DIV_DATA_OUT::MONTH_T;
                else if(input[i]=="d")
                    extract_div_data = DIV_DATA_OUT::DAY_T;
                else if(input[i]=="lat")
                    extract_div_data = DIV_DATA_OUT::LAT;
                else if(input[i]=="lon")
                    extract_div_data = DIV_DATA_OUT::LON;
                else if(input[i]=="latlon")
                    extract_div_data = (DIV_DATA_OUT)(DIV_DATA_OUT::LAT|DIV_DATA_OUT::LON);
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

    if(!is_correct_interval(date_from,date_to))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"",AT_ERROR_ACTION::ABORT);
    
    if(mode_extract==DataExtractMode::POSITION){
        if(!is_correct_pos(&coord)) //actually for WGS84
            ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
        hExtract.execute();
    }
    // else if(mode_extract==DataExtractMode::RECT){
    //     if(!correct_rect(&rect))
    //         ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    //     extract_cpp_rect(in,date_from,date_to,rect,extract_out_fmt);
    // }
    else
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
}

std::vector<std::string_view> commands_from_extract_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands;
    commands.push_back(translate_from_token(translate::token::ModeArgs::EXTRACT));
    Properties props;
    DataExtractMode mode_extract = DataExtractMode::UNDEF;
    Extract::ExtractFormat output_format;
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
                props.from_date_ = get_date_from_token(input.at(i));
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::DATE_TO:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                props.to_date_ = get_date_from_token(input.at(i));
                commands.push_back(input.at(i));
                break;
            }
            // case translate::token::Command::LAT_TOP:{
            //     if(i>=input.size())
            //         ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
            //     try{
            //         rect.y1 = get_coord_from_token<double>(input[i],mode_extract);
            //         commands.push_back(input.at(i));
            //     }
            //     catch(const std::invalid_argument& err){
            //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
            //     }
            //     break;
            // }
            // case translate::token::Command::LAT_BOT:{
            //     if(i>=input.size())
            //         ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
            //     try{
            //         rect.y2 = get_coord_from_token<double>(input[i],mode_extract);
            //         commands.push_back(input.at(i));
            //     }
            //     catch(const std::invalid_argument& err){
            //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
            //     }
            //     break;
            // }
            // case translate::token::Command::LON_LEFT:{
            //     if(i>=input.size())
            //         ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
            //     try{
            //         rect.x1 = get_coord_from_token<double>(input[i],mode_extract);
            //         commands.push_back(input.at(i));
            //     }
            //     catch(const std::invalid_argument& err){
            //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
            //     }
            //     break;
            // }
            // case translate::token::Command::LON_RIG:{
            //     if(i>=input.size())
            //         ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
            //     try{
            //         rect.x2 = get_coord_from_token<double>(input[i],mode_extract);
            //         commands.push_back(input.at(i));
            //     }
            //     catch(const std::invalid_argument& err){
            //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,input[i]);
            //     }
            //     break;
            // }
            case translate::token::Command::EXTRACT_FORMAT:{
                if(i>=input.size())
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
                output_format = get_extract_format(input[i]);
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
                    extract_div_data = DIV_DATA_OUT::HOUR_T;
                else if(input[i]=="y")
                    extract_div_data = DIV_DATA_OUT::YEAR_T;
                else if(input[i]=="m")
                    extract_div_data = DIV_DATA_OUT::MONTH_T;
                else if(input[i]=="d")
                    extract_div_data = DIV_DATA_OUT::DAY_T;
                else if(input[i]=="lat")
                    extract_div_data = DIV_DATA_OUT::LAT;
                else if(input[i]=="lon")
                    extract_div_data = DIV_DATA_OUT::LON;
                else if(input[i]=="latlon")
                    extract_div_data = (DIV_DATA_OUT)(DIV_DATA_OUT::LAT|DIV_DATA_OUT::LON);
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
                props.position_ = get_coord_from_token<Coord>(input[i],mode_extract);
                commands.push_back(input.at(i));
                break;        
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::ABORT,input[i]);
                break;
            }
        }
    }
    if(is_correct_interval(props.from_date_,props.to_date_))
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"Date interval is defined incorrectly",AT_ERROR_ACTION::ABORT);
    
    if(mode_extract==DataExtractMode::POSITION){
        if(!props.position_.has_value())
            ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"Not defined",AT_ERROR_ACTION::ABORT);
        if(!is_correct_pos(&props.position_.value())) //actually for WGS84
            ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::ABORT);
        return commands;
    }
    // else if(mode_extract==DataExtractMode::RECT){
    //     if(!correct_rect(&rect))
    //         ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    //     return commands;
    // }
    else
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    return commands;
}