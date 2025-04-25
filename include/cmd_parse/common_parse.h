#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"
#include <thread>
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "functions.h"
#include "data/info.h"
#include "extract.h"
#include "code_tables/table_0.h"

ErrorCode common_parse(const std::vector<std::string_view>& input, Extract& obj){
    std::vector<std::pair<uint8_t,std::string_view>> aliases_parameters;
    for(size_t i=0;i<input.size();++i){
        switch (translate_from_txt<translate::token::Command>(input[i++]))
        {
            case translate::token::Command::THREADS:{
                auto tmp_proc_num = from_chars<long>(input[i]);
                if(!tmp_proc_num.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                obj.set_using_processor_cores(tmp_proc_num.value());
                break;
            }
            case translate::token::Command::IN_PATH:
                obj.set_from_path(input[i]);
                break;
            case translate::token::Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                }
                if(tokens.at(0)=="dir")
                    obj.set_dest_dir(tokens.at(1));
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
                obj.set_from_date(date.value());
                break;
            }
            case translate::token::Command::DATE_TO:{
                auto date = get_date_from_token(input[i]);
                if(!date.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                obj.set_to_date(date.value());
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
                obj.set_center(center.value());
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
                        obj.add_parameter(SearchParamTableVersion{ .param_=(uint8_t)parameter.value(),
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
                auto coord_tmp = get_coord_from_token(input[i]);
                if(!coord_tmp.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                obj.set_position(coord_tmp.value());
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
                obj.set_grid_respresentation((RepresentationType)grid_tmp.value());
                break;
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::CONTINUE,input[i]);
                return ErrorCode::COMMAND_INPUT_X1_ERROR;
                break;
            }
        }
    }
}