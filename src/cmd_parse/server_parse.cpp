#include <cmd_parse/server_parse.h>

using namespace translate::token;

ErrorCode server_parse(const std::vector<std::string_view>& input){
    std::vector<std::string_view> commands_saved;
    std::string_view config_name;
    ErrorCode err = ErrorCode::NONE; 
    switch(translate_from_txt<ServerAction>(input.at(0))){
        case ServerAction::ADD:
            break;
        case ServerAction::REMOVE :
            break;
        case ServerAction::LAUNCH:
            break;
        case ServerAction::SHUTDOWN:
            break;
        case ServerAction::CLOSE:
            if(hProgram)
                hProgram->
            break;
        case ServerAction::CONFIG:
            return server_config(input);
            break;
        default:
            return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined server mode argument",AT_ERROR_ACTION::CONTINUE,input.at(0));
    }
    return err;
}

ErrorCode server_config(const std::vector<std::string_view>& input){
    ErrorCode err = ErrorCode::NONE;
    for(int i=2;i<input.size();++i){
        switch(translate_from_txt<ServerConfigCommands>(input.at(2))){
            case ServerConfigCommands::ADDRESS:{
                
                break;
            }
            case ServerConfigCommands::PORT:{
                
                break;
            }
            case ServerConfigCommands::ACCEPTED_ADDRESSES:{
                
                break;
            }
            default:
                return ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined server config argument",AT_ERROR_ACTION::CONTINUE,input.at(2));
        }
    }
    return err;
}