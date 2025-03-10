#include "cmd_parse/config_parse.h"
#include <thread>

#include "sys/error_code.h"
#include "sys/error_print.h"
#include "capitalize.h"
#include "sys/config.h"
#include "cmd_parse/cmd_def.h"
#include "cmd_translator.h"
#include "cmd_parse/functions.h"
#include "functions.h"
#include "cmd_parse/capitalize_parse.h"
#include "cmd_parse/check_parse.h"
#include "cmd_parse/extract_parse.h"

void config_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands_saved;
    OrderItems order = OrderItems();
    MODE mode = MODE::NONE;

    switch(translate_from_txt<translate::token::ModeArgs>(input.at(1))){
        case translate::token::ModeArgs::EXTRACT:
            commands_saved = std::move(commands_from_extract_parse({input.begin()+1,input.end()}));
            break;
        case translate::token::ModeArgs::CAPITALIZE:
            commands_saved = std::move(commands_from_capitalize_parse({input.begin()+1,input.end()}));
            break;
        case translate::token::ModeArgs::CHECK:
            commands_saved = std::move(commands_from_check_parse({input.begin()+1,input.end()}));
            break;
        case translate::token::ModeArgs::CONFIG:
            ErrorPrint::print_error(ErrorCode::INCOR_ARG_X1_ALREADY_CHOOSEN_MODE_X2,"",AT_ERROR_ACTION::ABORT,input[1],get_string_mode(mode));
            break;
        default:
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::ABORT,input.at(1));
    }
}