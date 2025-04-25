#include "extract_parse.h"
#include <thread>
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "functions.h"
#include "data/info.h"
#include "extract.h"
#include "code_tables/table_0.h"
#include "cmd_parse/common_parse.h"

namespace fs = std::filesystem;

std::optional<Extract::ExtractFormat> get_extract_format(std::string_view input){
    Extract::ExtractFormat result=Extract::ExtractFormat::DEFAULT;
    std::vector<std::string_view> tokens = split(input,":");
    if(tokens.empty()){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid input at extract format definition",AT_ERROR_ACTION::CONTINUE,input);
        return std::nullopt;
    }
    else{
        for(std::string_view token:tokens){
            if(token.empty()){
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid input at extract format definition",AT_ERROR_ACTION::CONTINUE,input);
                return std::nullopt;
            }
            else
                switch (translate_from_txt<translate::token::FileFormat>(token))
                {
                case translate::token::FileFormat::ARCHIVED:{
                    if((result&Extract::ExtractFormat::ARCHIVED)!=Extract::ExtractFormat::DEFAULT)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Archive format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else
                        result|=Extract::ExtractFormat::ARCHIVED;
                    break;
                }
                case translate::token::FileFormat::BIN:{
                    Extract::ExtractFormat cur = result&~Extract::ExtractFormat::ARCHIVED;
                    if(cur==Extract::ExtractFormat::DEFAULT)
                        result|=Extract::ExtractFormat::BIN_F;
                    else if(cur==Extract::ExtractFormat::BIN_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Binary format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else{
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                        return std::nullopt;
                    }
                    break;
                }
                case translate::token::FileFormat::GRIB:{
                    Extract::ExtractFormat cur = result&~Extract::ExtractFormat::ARCHIVED;
                    if(cur==Extract::ExtractFormat::DEFAULT)
                        result|=Extract::ExtractFormat::GRIB_F;
                    else if(cur==Extract::ExtractFormat::GRIB_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Grib format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else{
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                        return std::nullopt;
                    }
                    break;
                }
                case translate::token::FileFormat::TXT:{
                    Extract::ExtractFormat cur = result &~Extract::ExtractFormat::ARCHIVED;
                    if(cur==Extract::ExtractFormat::DEFAULT)
                        result|=Extract::ExtractFormat::TXT_F;
                    else if(cur==Extract::ExtractFormat::TXT_F)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Text format already defined",AT_ERROR_ACTION::CONTINUE,token);
                    else{
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use two different extraction format (except archiving)",AT_ERROR_ACTION::ABORT,input);
                        return std::nullopt;
                    }
                    break;
                }
                default:
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for extract mode",AT_ERROR_ACTION::CONTINUE,token);
                    return std::nullopt;
                }
        }
        return result;
    }
}

#include <format>
#include <numeric>
ErrorCode extract_parse(const std::vector<std::string_view>& input){
    Extract hExtract;
    DataExtractMode mode_extract = DataExtractMode::UNDEF;
    std::vector<std::pair<uint8_t,std::string_view>> aliases_parameters;
    for(size_t i=0;i<input.size();++i){
        switch (translate_from_txt<translate::token::Command>(input[i++]))
        {
            case translate::token::Command::THREADS:{
                auto tmp_proc_num = from_chars<long>(input[i]);
                if(!tmp_proc_num.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                hExtract.set_using_processor_cores(tmp_proc_num.value());
                break;
            }
            case translate::token::Command::IN_PATH:
                hExtract.set_from_path(input[i]);
                break;
            case translate::token::Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                }
                if(tokens.at(0)=="dir")
                    hExtract.set_dest_dir(tokens.at(1));
                else if(tokens.at(0)=="ip"){
                //TODO
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use extract mode by IP",AT_ERROR_ACTION::CONTINUE,tokens[0]);
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for extract mode",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                }
                break;
            }
            case translate::token::Command::DATE_FROM:{
                auto date = get_date_from_token(input[i]);
                if(!date.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                hExtract.set_from_date(date.value());
                break;
            }
            case translate::token::Command::DATE_TO:{
                auto date = get_date_from_token(input[i]);
                if(!date.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                hExtract.set_to_date(date.value());
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
                auto tmp = get_extract_format(input[i]);
                if(!tmp.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                hExtract.set_output_format(tmp.value());
                break;
            }
            case translate::token::Command::EXTRACTION_TIME_INTERVAL:{
                auto interval = get_time_offset_from_token(input[i]);
                if(!interval.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                hExtract.set_offset_time_interval(interval.value());
                break;
            }
            case translate::token::Command::CENTER:{
                //if number is inputed
                std::optional<uint8_t> center_int = from_chars<uint8_t>(input[i]);
                std::optional<Organization> center;
                //if abbreviation
                if(!center_int.has_value())
                    center = abbr_to_center(input[i]);
                else
                    center = (Organization)center_int.value();
                if(!center.has_value()){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined center",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                }
                hExtract.set_center(center.value());
                break;
            }
            case translate::token::Command::TABLE_VERSION:{
                auto table_version=from_chars<int>(input[i]);
                if(!table_version.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                if(table_version.value()<0){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid table version definition",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                }
                //hExtract.set_table_version((uint8_t)table_version.value()); //TODO make parameter optional (if undefined - check all parameters from table)
                break;
            }
            case translate::token::Command::PARAMETERS:{
                auto parameters_input = split(input[i],",");
                for(const auto& param:parameters_input){
                    auto parameter_tv = split(param,":");
                    if(parameter_tv.size()!=2){
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid parameter-definition input",AT_ERROR_ACTION::CONTINUE,param);
                        return ErrorCode::COMMAND_INPUT_X1_ERROR;
                    }
                    auto table_version = from_chars<int>(parameter_tv.at(0));
                    if(!table_version.has_value())
                        return ErrorCode::COMMAND_INPUT_X1_ERROR;
                    auto parameter = from_chars<int>(parameter_tv.at(1));
                    if(parameter.has_value()){
                        if(parameter.value()<0 || table_version.value()<0){
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid parameter number definition",AT_ERROR_ACTION::CONTINUE,input[i]);
                            return ErrorCode::COMMAND_INPUT_X1_ERROR;
                        }
                        hExtract.add_parameter(SearchParamTableVersion{ .param_=(uint8_t)parameter.value(),
                                                                        .t_ver_=(uint8_t)table_version.value()}); //add parameter
                    }
                    else
                        aliases_parameters.push_back({table_version.value(),parameter_tv.at(1)});
                }
                break;
            }
            case translate::token::Command::COLLECTION:{
                //combination of tab_ver, center etc
                break;
            }
            case translate::token::Command::TIME_FCST:{
                //<, <=, =, >=, > with these operands
                break;
            }
            case translate::token::Command::POSITION:{
                auto coord_tmp = get_coord_from_token<Coord>(input[i],mode_extract);
                if(!coord_tmp.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                hExtract.set_position(coord_tmp.value());
                break;        
            }
            case translate::token::Command::GRID_TYPE:{
                auto grid_tmp = from_chars<int>(input[i]);
                if(!grid_tmp.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                if(!grid_tmp.has_value() || grid_tmp.value()<0){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid grid type",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                }
                hExtract.set_grid_respresentation((RepresentationType)grid_tmp.value());
                break;
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::CONTINUE,input[i]);
                return ErrorCode::COMMAND_INPUT_X1_ERROR;
                break;
            }
        }
    }
    if(!hExtract.from_file().empty() && !fs::exists(hExtract.from_file())){
        ErrorPrint::print_error(ErrorCode::INVALID_PATH_OF_DIRECTORIES_X1,"",AT_ERROR_ACTION::CONTINUE,hExtract.from_file().c_str());
        return ErrorCode::INVALID_PATH_OF_DIRECTORIES_X1;
    }

    if(!is_correct_interval(hExtract.date_from(),hExtract.date_to())){
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::INCORRECT_DATE_INTERVAL;
    }

    if(!hExtract.get_center().has_value()){
        ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Center",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::UNDEFINED_VALUE;
    }
    if(!aliases_parameters.empty()){
        std::set<int> error_pos;
        auto result = post_parsing_parameters_aliases_def(hExtract.get_center().value(),aliases_parameters,error_pos);
        if(!error_pos.empty()){
            std::string error_aliases;
            for(int pos:error_pos)
                error_aliases+=""s+aliases_parameters.at(pos).second.data()+",";
            error_aliases.pop_back();
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Parameter alias definition error",AT_ERROR_ACTION::CONTINUE,error_aliases);
            return ErrorCode::COMMAND_INPUT_X1_ERROR;
        }
        else{
            for(auto& param:result){
                hExtract.add_parameter(std::move(param));
            }
        }
    }
    if(!is_correct_interval(hExtract.date_from(),hExtract.date_to())){
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE,"",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::INCORRECT_DATE;
    }
    else if(!hExtract.get_pos().has_value()){
        ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Not defined",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::UNDEFINED_VALUE;
    }
    else if(!is_correct_pos(hExtract.get_pos().value())){
        ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::INCORRECT_COORD;
    }
    else if(!hExtract.get_grid_representation().has_value()){
        ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Grid type",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::UNDEFINED_VALUE;
    }
    else if(hExtract.get_parameters().empty()){
        ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Parameters",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::UNDEFINED_VALUE;
    }
    if(mode_extract==DataExtractMode::POSITION){
        if(!hExtract.get_pos().has_value() || !hExtract.get_pos()->is_correct_pos()){ //actually for WGS84
            ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::CONTINUE);
            return ErrorCode::INCORRECT_RECT;
        }
        return hExtract.execute();
    }
    // else if(mode_extract==DataExtractMode::RECT){
    //     if(!correct_rect(&rect))
    //         ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    //     extract_cpp_rect(in,date_from,date_to,rect,extract_out_fmt);
    // }
    else{
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::COMMAND_INPUT_X1_ERROR;
    }
}

std::vector<std::string_view> commands_from_extract_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands;
    commands.push_back(translate_from_token(translate::token::ModeArgs::EXTRACT));
    SearchProperties props;
    DataExtractMode mode_extract = DataExtractMode::UNDEF;
    std::optional<Extract::ExtractFormat> output_format;
    unsigned int cpus = std::thread::hardware_concurrency();

    for(size_t i=0;i<input.size();++i){
        switch (translate_from_txt<translate::token::Command>(commands.emplace_back(input[i++])))
        {
            case translate::token::Command::THREADS:{
                if(i>=input.size()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return {};
                }
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::IN_PATH:{
                if(i>=input.size()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return {};
                }
                if(!fs::is_directory(input.at(i))){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::CONTINUE,input.at(i));
                    return {};
                }
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::OUT_PATH:{
                if(i>=input.size()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return {};
                }
                std::vector<std::string_view> tokens = split(input.at(i),":");
                if(tokens.size()!=2){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input.at(i));
                    return {};
                }
                if(tokens.at(0)=="dir"){
                    if(!fs::is_directory(tokens.at(1))){
                        if(!fs::create_directory(tokens.at(1))){
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::CONTINUE,tokens.at(1));
                            return {};
                        }
                    }
                    commands.push_back(input.at(i));
                }
                else if(tokens.at(0)=="ip"){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use check mode by IP",AT_ERROR_ACTION::CONTINUE,tokens[0]);
                    return {};
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for check mode",AT_ERROR_ACTION::CONTINUE,input.at(i));
                    return {};
                }
                break;
            }
            case translate::token::Command::DATE_FROM:{
                if(i>=input.size()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return {};
                }
                auto from_date_tmp = get_date_from_token(input.at(i));
                if(!from_date_tmp.has_value())
                    return {};
                props.from_date_ = from_date_tmp.value();
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::DATE_TO:{
                if(i>=input.size()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return {};
                }
                auto to_date_tmp = get_date_from_token(input.at(i));
                if(!to_date_tmp.has_value())
                    return {};
                props.to_date_ = to_date_tmp.value();
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
                if(i>=input.size()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return {};
                } 
                output_format = get_extract_format(input[i]);
                if(!output_format.has_value())
                    return {};
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::EXTRACTION_TIME_INTERVAL:{
                if(i>=input.size()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    return {};
                }
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Extract division data format still not supported in this Mashroom's version",AT_ERROR_ACTION::CONTINUE,input[i]);
                return {};
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
            case translate::token::Command::GRID_TYPE:{
                auto grid_tmp = from_chars<int>(input[i]);
                if(!grid_tmp.has_value())
                    return {};
                if(grid_tmp<0){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid grid type",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return {};
                }
                props.grid_type_ = (RepresentationType)grid_tmp.value();
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::CONTINUE,input[i]);
                return {};
                break;
            }
        }
    }
    if(is_correct_interval(props.from_date_,props.to_date_)){
        ErrorPrint::print_error(ErrorCode::INCORRECT_DATE_INTERVAL,"Date interval is defined incorrectly",AT_ERROR_ACTION::CONTINUE);
        return {};
    }
    
    if(mode_extract==DataExtractMode::POSITION){
        if(!props.position_.has_value()){
            ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"Not defined",AT_ERROR_ACTION::CONTINUE);
            return {};
        }
        if(!is_correct_pos(&props.position_.value())){ //actually for WGS84
            ErrorPrint::print_error(ErrorCode::INCORRECT_COORD,"",AT_ERROR_ACTION::ABORT);
            return {};
        }
        return commands;
    }
    // else if(mode_extract==DataExtractMode::RECT){
    //     if(!correct_rect(&rect))
    //         ErrorPrint::print_error(ErrorCode::INCORRECT_RECT,"Rectangle zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::ABORT);
    //     return commands;
    // }
    else{
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined zone in extraction is not defined or is defined incorrectly",AT_ERROR_ACTION::CONTINUE);
        return {};
    }
    return commands;
}