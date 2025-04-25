#include "cmd_parse/capitalize_parse.h"
#include <thread>
#include <filesystem>
#include <iostream>
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "capitalize.h"
#include "cmd_parse/functions.h"
#include "capitalize.h"

namespace fs = std::filesystem;

void capitalize_parse(const std::vector<std::string_view>& input){
    using namespace translate::token;
    Capitalize hCapitalize;
    unsigned int cpus = std::thread::hardware_concurrency();
    fs::path in = fs::current_path();
    fs::path out = fs::current_path();
    ErrorCode err;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            case Command::THREADS:{
                auto tmp_proc_num = from_chars<long>(input[i]);
                if(!tmp_proc_num.has_value())
                    return;
                hCapitalize.set_using_processor_cores(tmp_proc_num.value());
                break;
            }
            case Command::IN_PATH:
                err = hCapitalize.set_from_path(input[i]);
                if(err!=ErrorCode::NONE){
                    ErrorPrint::print_error(err,"",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return;
                }
                break;
            case Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return;
                }
                if(tokens.at(0)=="dir")
                    hCapitalize.set_dest_dir(tokens.at(1));
                else if(tokens.at(0)=="ip"){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use capitalize mode by IP",AT_ERROR_ACTION::CONTINUE,tokens[0]);
                    return;
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::CONTINUE,input[i]);
                }
                break;
            }
            case Command::CAPITALIZE_HIERARCHY:
                hCapitalize.set_output_order(input[i]);
                break;
            case Command::CAPITALIZE_FORMAT:
                if(hCapitalize.output_format()==DataFormat::NONE){
                    switch(translate_from_txt<FileFormat>(input[i])){
                        case FileFormat::BIN:
                            hCapitalize.set_output_type(DataFormat::BINARY);
                            break;
                        case FileFormat::GRIB:
                            hCapitalize.set_output_type(DataFormat::GRIB);
                            break;
                        case FileFormat::TXT:
                            hCapitalize.set_output_type(DataFormat::TEXT);
                            break;
                        default:
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize format",AT_ERROR_ACTION::CONTINUE,input[i]);
                    }
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Already choosen other capitalize format",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return;
                }
                break;
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::CONTINUE,input[i]);
                return;
            }
        }
    }
    if(hCapitalize.output_format()==DataFormat::NONE){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Not defined output format for capitalize mode",AT_ERROR_ACTION::CONTINUE);
        return;
    }
    hCapitalize.execute();
}

std::vector<std::string_view> commands_from_capitalize_parse(const std::vector<std::string_view>& input){
    using namespace translate::token;
    std::vector<std::string_view> commands;
    std::string order;
    DataFormat fmt = DataFormat::NONE;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            case Command::THREADS:{
                auto tmp_proc_num = from_chars<long>(input[i]);
                if(!tmp_proc_num.has_value())
                    return {};
                commands.push_back(input[i]);
                break;
            }
            case Command::IN_PATH:{
                if(!fs::is_directory(input[i])){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return {};
                }
                else commands.push_back(input.at(i));
                break;
            }
            case Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return {};
                }
                if(tokens.at(0)=="dir"){
                    if(!fs::is_directory(tokens.at(1))){
                        if(!fs::create_directory(tokens.at(1))){
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::CONTINUE,tokens.at(1));
                            return {};
                        }
                    }
                    commands.push_back(input[i]);
                }
                else if(tokens.at(0)=="ip"){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use capitalize mode by IP",AT_ERROR_ACTION::CONTINUE,tokens[0]);
                    return {};
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return {};
                }
                break;
            }
            case Command::CAPITALIZE_HIERARCHY:{
                if(Capitalize::check_format(input[i]))
                    commands.push_back(input[i]);
                else {
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                    "invalid capitalize order",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return {};
                }
                break;
            }
            case Command::CAPITALIZE_FORMAT:{
                if(fmt==DataFormat::NONE){
                    switch(translate_from_txt<FileFormat>(input[++i])){
                        case FileFormat::BIN:
                            fmt = DataFormat::BINARY;
                            break;
                        case FileFormat::GRIB:
                            fmt = DataFormat::GRIB;
                            break;
                        case FileFormat::TXT:
                            fmt = DataFormat::TEXT;
                            break;
                        default:{
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize format",AT_ERROR_ACTION::CONTINUE,input[i]);
                            return {};
                        }
                    }
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Already choosen other capitalize format",AT_ERROR_ACTION::CONTINUE,input[i]);
                    return {};
                }
                commands.push_back(input[i]);
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::CONTINUE,input[i]);
                return {};
            }
        }
    }
    if(fmt==DataFormat::NONE){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Not defined output format for capitalize mode",AT_ERROR_ACTION::CONTINUE);
        return {};
    }
    if(commands.empty()){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Zero args for capitalize mode",AT_ERROR_ACTION::CONTINUE);
        return {};
    }
    return commands;
}