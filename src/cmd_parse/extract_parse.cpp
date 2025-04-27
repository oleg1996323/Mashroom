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
#include "search_process_parse.h"

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
#include <time_separation_parse.h>
ErrorCode extract_parse(const std::vector<std::string_view>& input){
    Extract hExtract;
    std::vector<std::pair<uint8_t,std::string_view>> aliases_parameters;
    for(size_t i=0;i<input.size();++i){
        switch (translate_from_txt<translate::token::Command>(input[i++]))
        {
            case translate::token::Command::EXTRACT_FORMAT:{
                auto tmp = get_extract_format(input[i]);
                if(!tmp.has_value())
                    return ErrorCode::COMMAND_INPUT_X1_ERROR;
                hExtract.set_output_format(tmp.value());
                break;
            }
            case translate::token::Command::EXTRACTION_TIME_INTERVAL:{
                auto err = time_separation_parse(input[i],hExtract);
                if(err!=ErrorCode::NONE)
                    return err;
                break;
            }
            default:{
                auto err = search_process_parse(input[i-1],input[i],hExtract,aliases_parameters);
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::CONTINUE,input[i]);
                return ErrorCode::COMMAND_INPUT_X1_ERROR;
                break;
            }
        }
    }
    if(!hExtract.get_center().has_value()){
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
    }
    else{
        ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Center",AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::UNDEFINED_VALUE;
    }
    ErrorCode err = hExtract.properties_integrity();
    if(err!=ErrorCode::NONE)
        return err;
    return hExtract.execute();
}

std::vector<std::string_view> commands_from_extract_parse(const std::vector<std::string_view>& input,ErrorCode& err){
    std::vector<std::string_view> commands;
    std::vector<std::pair<uint8_t,std::string_view>> aliases_parameters;
    Extract hExtract;
    commands.push_back(translate_from_token(translate::token::ModeArgs::EXTRACT));
    for(size_t i=0;i<input.size();++i){
        switch (translate_from_txt<translate::token::Command>(commands.emplace_back(input[i++])))
        {
            case translate::token::Command::EXTRACT_FORMAT:{
                if(i>=input.size()){
                    ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::CONTINUE);
                    err = ErrorCode::TO_FEW_ARGUMENTS;
                    return {};
                } 
                auto out_fmt = get_extract_format(input[i]);
                if(!out_fmt.has_value()){
                    err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                    return {};
                }
                hExtract.set_output_format(out_fmt.value());
                commands.push_back(input.at(i));
                break;
            }
            case translate::token::Command::EXTRACTION_TIME_INTERVAL:{
                auto res = commands_from_time_separation(input[i],err);
                if(err!=ErrorCode::NONE || res.empty())
                    return {};
                else commands.push_back(res);
                break;
            }
            default:{
                commands_from_search_process(input[i-1],input[i],hExtract,aliases_parameters,err);
                if(err!=ErrorCode::NONE)
                    return {};
                else{
                    commands.push_back(input[i]);
                    break;
                }
                err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(input[i])+"]",AT_ERROR_ACTION::CONTINUE,input[i]);
                return {};
                break;
            }
        }
    }
    
    if(!hExtract.get_center().has_value()){
        if(!aliases_parameters.empty()){
            std::set<int> error_pos;
            auto result = post_parsing_parameters_aliases_def(hExtract.get_center().value(),aliases_parameters,error_pos);
            if(!error_pos.empty()){
                std::string error_aliases;
                for(int pos:error_pos)
                    error_aliases+=""s+aliases_parameters.at(pos).second.data()+",";
                error_aliases.pop_back();
                err = ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Parameter alias definition error",AT_ERROR_ACTION::CONTINUE,error_aliases);
                return {};
            }
            else{
                for(auto& param:result){
                    hExtract.add_parameter(std::move(param));
                }
            }
        }
    }
    else{
        err = ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Center",AT_ERROR_ACTION::CONTINUE);
        return {};
    }
    if(err!=ErrorCode::NONE || hExtract.properties_integrity()!=ErrorCode::NONE)
        return {};
    return commands;
}