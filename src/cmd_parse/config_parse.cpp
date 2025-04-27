#include "cmd_parse/config_parse.h"
#include <thread>

#include "sys/error_code.h"
#include "sys/error_print.h"
#include "capitalize.h"
#include "sys/application.h"
#include "sys/config.h"
#include "cmd_parse/cmd_def.h"
#include "cmd_translator.h"
#include "cmd_parse/functions.h"
#include "functions.h"
#include "cmd_parse/capitalize_parse.h"
#include "cmd_parse/integrity_parse.h"
#include "cmd_parse/extract_parse.h"

using namespace translate::token;

ErrorCode config_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands_saved;
    ConfigAction action = ConfigAction::UNDEF;
    std::string_view config_name;
    ErrorCode err = ErrorCode::NONE; 
    switch(translate_from_txt<ConfigAction>(input.at(0))){
        case ConfigAction::ADD:
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::ADD;
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
        case ConfigAction::REMOVE :{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::REMOVE;
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
        }
        case ConfigAction::ADDSET:{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::ADDSET;
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
        }
        case ConfigAction::SET:{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::SET;
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
            if(!input.at(1).starts_with('-'))
                config_name = input.at(1);
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
            if(app().config().has_config_name(input.at(1)))
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Already defined user-commands config name",AT_ERROR_ACTION::CONTINUE,input.at(1));
            break;
        }
        case ConfigAction::REDEFINE :{
            if(action==ConfigAction::UNDEF){
                action = ConfigAction::REDEFINE;
            }
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Unknown argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
            if(!input.at(1).starts_with('-'))
                config_name = input.at(1);
            else
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(1));
        }
        case ConfigAction::GET:{
            if(input.size()>1)
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"",AT_ERROR_ACTION::CONTINUE,input.at(1)); 
            if(input.at(1)=="."){
                for(auto& [name,commands]:app().config().get_user_configs()){
                    std::cout<<name<<"-> ";
                    for(const std::string& command:commands){
                        std::cout<<command<<' ';
                    }
                    std::cout.seekp(-1);
                    std::cout<<std::endl;
                }
            }
            else if(!input.at(1).starts_with('-')){
                if(!app().config().has_config_name(input.at(1)))
                    return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"User-commands config name doesn't exists",AT_ERROR_ACTION::CONTINUE,input.at(1));
                std::cout<<input.at(1)<<"-> ";
                for(auto& command:app().config().get_user_config(input.at(1))){
                    std::cout<<command<<' ';
                    std::cout.seekp(-1);
                    std::cout<<std::endl;
                }
            }
            else 
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(1));

            }
        default:{
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(0));
            return ErrorCode::COMMAND_INPUT_X1_ERROR;
        }
    }

    switch(translate_from_txt<translate::token::ModeArgs>(input.at(2))){
        case translate::token::ModeArgs::EXTRACT:{
            auto result = commands_from_extract_parse({input.begin()+3,input.end()},err);
            if(err!=ErrorCode::NONE)
                return err;
            Application::config().add_user_config(config_name,std::move(result));
            break;
        }
        case translate::token::ModeArgs::CAPITALIZE:{
            auto result = commands_from_capitalize_parse({input.begin()+3,input.end()},err);
            if(err!=ErrorCode::NONE)
                return err;
            Application::config().add_user_config(config_name,std::move(result));
            break;
        }
        case translate::token::ModeArgs::INTEGRITY:{
            auto result = commands_from_integrity_parse({input.begin()+3,input.end()},err);
            if(err!=ErrorCode::NONE)
                return err;
            Application::config().add_user_config(config_name,std::move(result));
            break;
        }
        case translate::token::ModeArgs::CONFIG:
            return ErrorPrint::print_error(ErrorCode::INCOR_ARG_X1_ALREADY_CHOOSEN_MODE_X2,"",AT_ERROR_ACTION::CONTINUE,input.at(2),get_string_mode(MODE::CONFIG));
            break;
        default:
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::CONTINUE,input.at(2));
    }
    if(err!=ErrorCode::NONE)
        return err;
    Application::config().add_user_config(config_name,commands_saved);
    return err;
}