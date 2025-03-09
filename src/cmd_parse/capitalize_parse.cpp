#include "cmd_parse/capitalize_parse.h"
#include <thread>
#include <filesystem>
#include <iostream>
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "capitalize_cpp.h"
#ifdef __cplusplus
    extern "C"{
        #include "capitalize.h"
    }
#endif

namespace fs = std::filesystem;

OrderItems get_item_orders(std::string_view input){
    OrderItems order = OrderItems();
    int order_count = 0;
    std::vector<std::string_view> tokens = split(std::string_view(input),":");
    for(std::string_view token:tokens){
        if(token.size()>0){
            switch(translate_cap_hierarchy(token)){
                case CapitalizeHierArgs::HOUR:
                    if(order.hour!=-1)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for hour",AT_ERROR_ACTION::CONTINUE,token);
                    else order.hour = order_count++;
                    break;
                case CapitalizeHierArgs::DAY:
                    if(order.day!=-1)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for day",AT_ERROR_ACTION::CONTINUE,token);
                    else order.day = order_count++;
                case CapitalizeHierArgs::MONTH:
                    if(order.month!=-1)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for month",AT_ERROR_ACTION::CONTINUE,token);
                    else order.month = order_count++;
                    break;
                case CapitalizeHierArgs::YEAR:
                    if(order.year!=-1)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for year",AT_ERROR_ACTION::CONTINUE,token);
                    else order.year = order_count++;
                    break;
                case CapitalizeHierArgs::LAT:
                    if(order.lat!=-1)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for latitude",AT_ERROR_ACTION::CONTINUE,token);
                    else order.lat = order_count++;
                    break;
                case CapitalizeHierArgs::LON:
                    if(order.lon!=-1)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for longitude",AT_ERROR_ACTION::CONTINUE,token);
                    else order.lon = order_count++;
                    break;
                case CapitalizeHierArgs::LATLON:
                    if(order.lon!=-1 && order.lat!=-1)
                        ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for longitude",AT_ERROR_ACTION::CONTINUE,token);
                    else{
                        order.lat = order_count;
                        order.lon = order_count++;
                    }
                    break;
                default:{
                    ErrorPrint::message(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize hierarchy order",token);
                }
            }
        }
    }
    return order;
}

void capitalize_parse(std::vector<std::string_view> input){
    unsigned int cpus = std::thread::hardware_concurrency();
    fs::path in = fs::current_path();
    fs::path out = fs::current_path();
    OrderItems order = OrderItems();
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt(input[i++])){
            case::Command::THREADS:{
                long tmp = from_chars<long>(input[++i]);
                if(tmp>=1 & tmp<=std::thread::hardware_concurrency())
                    cpus = tmp;
                break;
            }
            case Command::IN_PATH:{
                in = input[i];
                if(!fs::is_directory(in))
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,in.c_str());
                break;
            }
            case Command::OUT_PATH:{
                std::vector<std::string_view> tokens = split(input[i],":");
                if(tokens.size()!=2)
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::ABORT,input[i]);
                if(tokens.at(0)=="dir"){
                    out = tokens.at(1);
                    if(!fs::is_directory(out)){
                        if(!fs::create_directory(out))
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not directory",AT_ERROR_ACTION::ABORT,out.c_str());
                    }
                }
                else if(tokens.at(0)=="ip")
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unable to use capitalize mode by IP",AT_ERROR_ACTION::ABORT,tokens[0]);
                else
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::ABORT,input[i]);
                break;
            }
            case Command::CAPITALIZE_HIERARCHY:{
                order = get_item_orders(input[i]);
                break;
            }
            case Command::CAPITILIZE_FORMAT:{
                if(order.fmt==NONE){
                    switch(translate_cap_format(input[++i])){
                        case CapitilizeFormatArgs::BIN:
                            order.fmt = BINARY;
                            break;
                        case CapitilizeFormatArgs::GRIB:
                            order.fmt = GRIB;
                            break;
                        case CapitilizeFormatArgs::TXT:
                            order.fmt = TEXT;
                            break;
                        default:
                            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize format",AT_ERROR_ACTION::ABORT,input[i]);
                    }
                }
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Already choosen other capitalize format",AT_ERROR_ACTION::ABORT,input[i]);
                }
            }
            default:{
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize mode",AT_ERROR_ACTION::ABORT,input[i]);
            }
        }
    }
    capitalize_cpp(in,out,order);
}