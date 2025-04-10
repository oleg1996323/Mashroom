#include "cmd_parse/functions.h"
#include "error_print.h"
#include <iostream>

std::string get_string_mode(MODE mode)
{
    switch(mode){
        case(MODE::NONE):
            return "None mode";
        case(MODE::CAPITALIZE):
            return "Capitalize mode (arg = -cap)";
        case(MODE::CHECK_ALL_IN_PERIOD):
            return "Check mode (arg = -check)";
        case(MODE::EXTRACT):
            return "Extract mode (arg = -ext)";
        default:{
            std::cout<<"Argument error at \"get_string_mode\""<<std::endl;
            exit(1);
        }
    }
}
std::vector<std::string> split(const std::string& str,const char* delimiter){
    std::vector<std::string> result;
    size_t beg_pos = 0;
    size_t pos = beg_pos;
    while(true){
        pos=str.find_first_of(delimiter,pos);
        if(pos!=std::string::npos){
            result.push_back(str.substr(beg_pos,pos-beg_pos));
            beg_pos=pos;
        }
        else return result;
    }
}
std::vector<std::string_view> split(std::string_view str, const char* delimiter){
    std::vector<std::string_view> result;
    size_t beg_pos = 0;
    size_t pos = 0;
    while(pos!=std::string::npos){
        pos = str.find_first_of(delimiter,beg_pos);
        result.push_back(str.substr(beg_pos,pos-beg_pos));
        beg_pos=pos+1;
    }
    return result;
}

Date get_date_from_token(std::string_view input){
    Date result = Date();
    std::vector<std::string_view> tokens = split(std::string_view(input),":");
    if(!tokens.empty())
        for(std::string_view token:tokens){
            if(token.size()>0){
                if(token.starts_with("h")){
                    result.hour = from_chars<int>(token.substr(1));
                }
                else if(token.starts_with("y")){
                    result.year = from_chars<int>(token.substr(1));
                }
                else if(token.starts_with("m")){
                    result.month = from_chars<int>(token.substr(1));
                }
                else if(token.starts_with("d")){
                    result.day = from_chars<int>(token.substr(1));
                }
                else
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown token for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,token);
            }
            else
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed token for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,input);
        }
    else
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missed tokens for extraction mode hierarchy",AT_ERROR_ACTION::ABORT,input);
    return result;
}

namespace functions::capitalize{
    OrderItems get_item_orders(std::string_view input){
        using namespace translate::token;
        OrderItems order = OrderItems();
        int order_count = 0;
        std::vector<std::string_view> tokens = split(std::string_view(input),":");
        for(std::string_view token:tokens){
            if(token.size()>0){
                switch(translate_from_txt<CategoryClassArgs>(token)){
                    case CategoryClassArgs::HOUR:
                        if(order.hour!=-1)
                            ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for hour",AT_ERROR_ACTION::CONTINUE,token);
                        else order.hour = order_count++;
                        break;
                    case CategoryClassArgs::DAY:
                        if(order.day!=-1)
                            ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for day",AT_ERROR_ACTION::CONTINUE,token);
                        else order.day = order_count++;
                        break;
                    case CategoryClassArgs::MONTH:
                        if(order.month!=-1)
                            ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for month",AT_ERROR_ACTION::CONTINUE,token);
                        else order.month = order_count++;
                        break;
                    case CategoryClassArgs::YEAR:
                        if(order.year!=-1)
                            ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for year",AT_ERROR_ACTION::CONTINUE,token);
                        else order.year = order_count++;
                        break;
                    case CategoryClassArgs::LAT:
                        if(order.lat!=-1)
                            ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for latitude",AT_ERROR_ACTION::CONTINUE,token);
                        else order.lat = order_count++;
                        break;
                    case CategoryClassArgs::LON:
                        if(order.lon!=-1)
                            ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for longitude",AT_ERROR_ACTION::CONTINUE,token);
                        else order.lon = order_count++;
                        break;
                    case CategoryClassArgs::LATLON:
                        if(order.lon!=-1 && order.lat!=-1)
                            ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"Already choosen order hierarchy for longitude",AT_ERROR_ACTION::CONTINUE,token);
                        else{
                            order.lat = order_count;
                            order.lon = order_count++;
                        }
                        break;
                    default:{
                        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument for capitalize hierarchy order",AT_ERROR_ACTION::ABORT,token);
                    }
                }
            }
        }
        return order;
    }

    std::string get_txt_order(const OrderItems& order){
        std::vector<int OrderItems::*> items{
        &OrderItems::day,
        &OrderItems::hour,
        &OrderItems::lat,
        &OrderItems::lon,
        &OrderItems::month,
        &OrderItems::year};
        int max = std::max({order.day,order.hour,order.lat,order.lon,order.month,order.year});
        int current_order = 0;
        std::string res;
        while(current_order<=max){
            if(order.day==current_order)
                res.push_back('D');
            else if(order.hour==current_order)
                res.push_back('H');
            else if(order.month==current_order)
                res.push_back('M');
            else if(order.year==current_order)
                res.push_back('Y');
            else if(order.lat==current_order)
                if(order.lat==order.lon)
                    res.push_back('C');
                else res.push_back('L');
            else if(order.lon==current_order)
                res.push_back('O');
            ++current_order;
        }
        return res.c_str();
    }

    OrderItems get_order_txt(std::string_view txt){
        OrderItems order;
        for(int i=0;i<txt.size();++i){
            switch (txt[i])
            {
            case 'd':
            case 'D':
                order.day = i;
            case 'h':
            case 'H':
                order.hour = i;
                break;
            case 'm':
            case 'M':
                order.month = i;
                break;
            case 'y':
            case 'Y':
                order.year = i;
                break;
            case 'c':
            case 'C':
                order.lat = i;
                order.lon = i;
                break;
            case 'l':
            case 'L':
                order.lat = i;
                break;
            case 'o':
            case 'O':
                order.lon = i;
                break;
            default:
                break;
            }
        }
        return order;
    }
}