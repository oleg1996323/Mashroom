#include <search_process_parse.h>

ErrorCode search_process_parse(std::string_view option,std::string_view arg, AbstractSearchProcess& obj,std::vector<std::pair<uint8_t,std::string_view>>& aliases_parameters){
    ErrorCode err = ErrorCode::NONE;
    switch (translate_from_txt<translate::token::Command>(option))
    {
        case translate::token::Command::THREADS:{
            auto tmp_proc_num = from_chars<long>(arg,err);
            if(!tmp_proc_num.has_value())
                break;
            obj.set_using_processor_cores(tmp_proc_num.value());
            break;
        }
        /** \brief Set user defined path(s) where iteratively (if file is not capitalized) or 
         *  directly (by file message pointers) search or  set user defined host(s) in the way
         *  to request the searched data
         * 
         */
        case translate::token::Command::IN_PATH:{
            std::vector<std::string_view> tokens;
            if(arg.starts_with('[') && arg.ends_with(']'))
                tokens = split(arg.substr(1,arg.size()-1),",");
            else
                tokens.push_back(arg);
            for(auto token:tokens){
                if(token.starts_with("path:"))
                    obj.add_in_path(token.substr(4));
                else if(token.starts_with("host:"))
                    obj.add_search_host(token.substr(5));
                else{
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,arg);
                    err = ErrorCode::COMMAND_INPUT_X1_ERROR;
                    break;
                }
            }
            break;
        }
        case translate::token::Command::OUT_PATH:{ //TODO: swap this block with IN_PATH
            err = obj.set_out_path(arg);
            if(err!=ErrorCode::NONE)
                break;
            else return err;
            break;
        }
        case translate::token::Command::DATE_FROM:{
            auto date = get_date_from_token(arg,err);
            if(!date.has_value())
                break;
            obj.set_from_date(date.value());
            break;
        }
        case translate::token::Command::DATE_TO:{
            auto date = get_date_from_token(arg,err);
            if(!date.has_value())
                break;
            obj.set_to_date(date.value());
            break;
        }
        // case translate::token::Command::LAT_TOP:{
        //     try{
        //         rect.y1 = get_coord_from_token<double>(arg,mode_extract);
        //     }
        //     catch(const std::invalid_argument& err){
        //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,arg);
        //     }
        //     break;
        // }
        // case translate::token::Command::LAT_BOT:{
        //     try{
        //         rect.y2 = get_coord_from_token<double>(arg,mode_extract);
        //     }
        //     catch(const std::invalid_argument& err){
        //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,arg);
        //     }
        //     break;
        // }
        // case translate::token::Command::LON_LEFT:{
        //     try{
        //         rect.x1 = get_coord_from_token<double>(arg,mode_extract);
        //     }
        //     catch(const std::invalid_argument& err){
        //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,arg);
        //     }
        //     break;
        // }
        // case translate::token::Command::LON_RIG:{
        //     try{
        //         rect.x2 = get_coord_from_token<double>(arg,mode_extract);
        //     }
        //     catch(const std::invalid_argument& err){
        //         ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"cannot convert to double value",AT_ERROR_ACTION::ABORT,arg);
        //     }
        //     break;
        // }
        case translate::token::Command::CENTER:{
            //if number is inputed
            std::optional<uint8_t> center_int = from_chars<uint8_t>(arg,err);
            std::optional<Organization> center;
            //if abbreviation
            if(!center_int.has_value())
                center = abbr_to_center(arg);
            else
                center = (Organization)center_int.value();
            if(!center.has_value()){
                break;
            }
            obj.set_center(center.value());
            break;
        }
        case translate::token::Command::TABLE_VERSION:{
            auto table_version=from_chars<int>(arg,err);
            if(!table_version.has_value())
                break;
            if(table_version.value()<0){
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid table version definition",AT_ERROR_ACTION::CONTINUE,arg);
                break;
            }
            //hExtract.set_table_version((uint8_t)table_version.value()); //TODO make parameter optional (if undefined - check all parameters from table)
            break;
        }
        case translate::token::Command::PARAMETERS:{
            auto parameters_input = split(arg,",");
            for(const auto& param:parameters_input){
                auto parameter_tv = split(param,":");
                if(parameter_tv.size()!=2){
                    return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid parameter-definition input",AT_ERROR_ACTION::CONTINUE,param);
                    break;
                }
                auto table_version = from_chars<int>(parameter_tv.at(0),err);
                if(!table_version.has_value())
                    break;
                auto parameter = from_chars<int>(parameter_tv.at(1),err);
                if(parameter.has_value()){
                    if(parameter.value()<0 || table_version.value()<0){
                        return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid parameter number definition",AT_ERROR_ACTION::CONTINUE,arg);
                        break;
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
            auto coord_tmp = get_coord_from_token(arg,err);
            if(!coord_tmp.has_value()){
                break;
            }
            obj.set_position(coord_tmp.value());
            break;
        }
        case translate::token::Command::GRID_TYPE:{
            auto grid_tmp = from_chars<int>(arg,err);
            if(!grid_tmp.has_value()){
                break;
            }
            if(!grid_tmp.has_value() || grid_tmp.value()<0){
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid grid type",AT_ERROR_ACTION::CONTINUE,arg);
                break;
            }
            obj.set_grid_respresentation((RepresentationType)grid_tmp.value());
            break;
        }
        default:
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Invalid argument: argv["+std::string(arg)+"]",AT_ERROR_ACTION::CONTINUE,arg);
            break;
    }
    if(err!=ErrorCode::NONE)
        return err;
    if(!obj.get_center().has_value())
        return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"Center",AT_ERROR_ACTION::CONTINUE);
    if(!aliases_parameters.empty()){
        std::set<int> error_pos;
        auto result = post_parsing_parameters_aliases_def(obj.get_center().value(),aliases_parameters,error_pos);
        if(!error_pos.empty()){
            std::string error_aliases;
            for(int pos:error_pos)
                error_aliases+=""s+aliases_parameters.at(pos).second.data()+",";
            error_aliases.pop_back();
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Parameter alias definition error",AT_ERROR_ACTION::CONTINUE,error_aliases);
        }
        else{
            for(auto& param:result){
                obj.add_parameter(std::move(param));
            }
        }
    }
    return err;
}

void commands_from_search_process(std::string_view option,std::string_view arg,AbstractSearchProcess& obj,std::vector<std::pair<uint8_t,std::string_view>>& aliases_parameters, ErrorCode& err){
    err = search_process_parse(option,arg,obj,aliases_parameters);
}