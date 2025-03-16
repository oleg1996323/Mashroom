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
#include "cmd_parse/check_parse.h"
#include "cmd_parse/extract_parse.h"

using namespace translate::token;

void config_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands_saved;
    OrderItems order = OrderItems();
    ConfigAction action = ConfigAction::UNDEF;
    std::string_view config_name;
    if(input.size()<4)
        ErrorPrint::print_error(ErrorCode::TO_FEW_ARGUMENTS,"",AT_ERROR_ACTION::ABORT);
    switch(translate_from_txt<ConfigAction>(input.at(0))){
        case ConfigAction::ADD:
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::ADD;
        case ConfigAction::REMOVE :{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::REMOVE;
        }
        case ConfigAction::ADDSET:{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::ADDSET;
        }
        case ConfigAction::SET:{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::SET;
            if(!input.at(1).starts_with('-'))
                config_name = input.at(1);
            else
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::ABORT,input.at(1));
            if(app().config().has_config_name(input.at(1)))
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Already defined user-commands config name",AT_ERROR_ACTION::ABORT,input.at(1));
            break;
        }
        case ConfigAction::REDEFINE :{
            if(action==ConfigAction::UNDEF)
                action = ConfigAction::REDEFINE;
            if(!input.at(1).starts_with('-'))
                config_name = input.at(1);
            else
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::ABORT,input.at(1));
        }
        case ConfigAction::GET:{
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
                    ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"User-commands config name doesn't exists",AT_ERROR_ACTION::ABORT,input.at(1));
                std::cout<<input.at(1)<<"-> ";
                for(auto& command:app().config().get_user_config(input.at(1))){
                    std::cout<<command<<' ';
                    std::cout.seekp(-1);
                    std::cout<<std::endl;
                }
            }
            else 
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::ABORT,input.at(1));

            for(int i=2;i<input.size();++i)
                ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"",AT_ERROR_ACTION::CONTINUE,input.at(i));
            return;
        }
        default:{
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::ABORT,input.at(0));
        }
    }

    
    switch(translate_from_txt<translate::token::ModeArgs>(input.at(2))){
        case translate::token::ModeArgs::EXTRACT:
            commands_saved = std::move(commands_from_extract_parse({input.begin()+3,input.end()}));
            break;
        case translate::token::ModeArgs::CAPITALIZE:
            commands_saved = std::move(commands_from_capitalize_parse({input.begin()+3,input.end()}));
            break;
        case translate::token::ModeArgs::CHECK:
            commands_saved = std::move(commands_from_check_parse({input.begin()+3,input.end()}));
            break;
        case translate::token::ModeArgs::CONFIG:
            ErrorPrint::print_error(ErrorCode::INCOR_ARG_X1_ALREADY_CHOOSEN_MODE_X2,"",AT_ERROR_ACTION::ABORT,input.at(2),get_string_mode(MODE::CONFIG));
            break;
        default:
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::ABORT,input.at(2));
    }
}