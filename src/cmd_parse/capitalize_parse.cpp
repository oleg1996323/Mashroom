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
#include "information_parse.h"

namespace fs = std::filesystem;

ErrorCode capitalize_parse(const std::vector<std::string_view>& input){
    using namespace translate::token;
    Capitalize hCapitalize;
    unsigned int cpus = std::thread::hardware_concurrency();
    ErrorCode err = ErrorCode::NONE;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            case Command::THREADS:{
                auto tmp_proc_num = from_chars<long>(input[i],err);
                if(!tmp_proc_num.has_value())
                    return err;
                hCapitalize.set_using_processor_cores(tmp_proc_num.value());
                break;
            }
            case Command::IN_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2)
                    return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input[i]);
                if(tokens.at(0)=="dir")
                    hCapitalize.add_in_path(path::Storage<true>(tokens.at(1),path::TYPE::DIRECTORY));
                else if(tokens.at(0)=="host")
                    hCapitalize.add_in_path(path::Storage<true>(tokens.at(1),path::TYPE::HOST));
                else if(tokens.at(0)=="file")
                    hCapitalize.add_in_path(path::Storage<true>(tokens.at(1),path::TYPE::FILE));
                break;
            }
            case Command::OUT_PATH:{
                err = hCapitalize.set_dest_dir(input[i]);
                if(err!=ErrorCode::NONE)
                    return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input[i]);
                else
                    hCapitalize.set_dest_dir(input[i]); //TODO add check extension and create file
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
                            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize format",AT_ERROR_ACTION::CONTINUE,input[i]);
                    }
                }
                else
                    return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Already choosen other capitalize format",AT_ERROR_ACTION::CONTINUE,input[i]);
                break;
            case Command::REF:
                hCapitalize.set_host_ref_only();
                break;
            case Command::PART:{
                if(auto res_unit_sz = parse::info_size_unit(input[i]);res_unit_sz.has_value())
                    hCapitalize.set_max_cap_size(res_unit_sz.value());
                else if(auto res_sz = parse::info_size(input[i]);res_sz.has_value()){
                    if(std::isinf(res_sz.value()))
                        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,
                            "too huge value",AT_ERROR_ACTION::CONTINUE,input[i]);
                    else if(auto res_unit = parse::info_unit(input[i+1]);res_unit.has_value())
                        hCapitalize.set_max_cap_size(res_unit.value()*static_cast<double>(res_sz.value()));
                    else return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"expected number and data unit (B,KB,MB,GB,TB, bytes, kilobyte etc.). Example, 1000MB or 1000 Megabytes.",AT_ERROR_ACTION::CONTINUE,input[i]);
                }
                break;
            }
            default:
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::CONTINUE,input[i]);
        }
    }
    if(hCapitalize.output_format()==DataFormat::NONE)
        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Not defined output format for capitalize mode",AT_ERROR_ACTION::CONTINUE);
    hCapitalize.execute();
    return err;
}

std::vector<std::string_view> commands_from_capitalize_parse(const std::vector<std::string_view>& input,ErrorCode& err){
    using namespace translate::token;
    std::vector<std::string_view> commands;
    std::string order;
    DataFormat fmt = DataFormat::NONE;
    for(size_t i=0;i<input.size();++i){
        if(err!=ErrorCode::NONE)
            return {};
        switch(translate_from_txt<Command>(input[i++])){
            case Command::THREADS:{
                auto tmp_proc_num = from_chars<long>(input[i],err);
                if(!tmp_proc_num.has_value())
                    return {};
                commands.push_back(input[i]);
                break;
            }
            case Command::IN_PATH:{
                if(!fs::is_directory(input[i])){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::CONTINUE,input[i]);
                    err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                    return {};
                }
                else commands.push_back(input.at(i));
                break;
            }
            case Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input[i]);
                    err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                    return {};
                }
                if(tokens.at(0)=="dir"){
                    if(!fs::is_directory(tokens.at(1))){
                        if(!fs::create_directory(tokens.at(1))){
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::CONTINUE,tokens.at(1));
                            err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                            return {};
                        }
                    }
                    commands.push_back(input[i]);
                }
                else if(tokens.at(0)=="ip"){
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use capitalize mode by IP",AT_ERROR_ACTION::CONTINUE,tokens[0]);
                    err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                    return {};
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::CONTINUE,input[i]);
                    err = ErrorCode::COMMAND_INPUT_X1_ERROR;
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
                    err = ErrorCode::COMMAND_INPUT_X1_ERROR;
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
                            err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                            return {};
                        }
                    }
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Already choosen other capitalize format",AT_ERROR_ACTION::CONTINUE,input[i]);
                    err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                    return {};
                }
                commands.push_back(input[i]);
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::CONTINUE,input[i]);
                err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                return {};
            }
        }
    }
    if(fmt==DataFormat::NONE){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Not defined output format for capitalize mode",AT_ERROR_ACTION::CONTINUE);
        err = ErrorCode::COMMAND_INPUT_X1_ERROR;
        return {};
    }
    if(commands.empty()){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Zero args for capitalize mode",AT_ERROR_ACTION::CONTINUE);
        err = ErrorCode::COMMAND_INPUT_X1_ERROR;
        return {};
    }
    return commands;
}