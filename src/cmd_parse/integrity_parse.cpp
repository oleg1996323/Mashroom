#include "cmd_parse/integrity_parse.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "integrity.h"
#include "cmd_parse/functions.h"
#include "search_process_parse.h"

ErrorCode integrity_parse(const std::vector<std::string_view>& input){
    using namespace translate::token;
    Integrity hIntegrity;
    std::vector<std::pair<uint8_t,std::string_view>> aliases_parameters;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            default:{
                auto err = search_process_parse(input[i-1],input[i],hIntegrity,aliases_parameters);
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"unknown \"Check\" mode argument",AT_ERROR_ACTION::CONTINUE,input[i]);
                return ErrorCode::COMMAND_INPUT_X1_ERROR;
            }
        }
    }
    auto err = hIntegrity.properties_integrity();
    if(err!=ErrorCode::NONE)
        return err;
    err = hIntegrity.execute();
    if(err!=ErrorCode::NONE)
        std::cout<<"Missing files are placed to file "<<hIntegrity.out_path()/"missing_files.txt"<<std::endl;
    else std::cout<<"No missing files detected"<<std::endl;
    return err;
}

std::vector<std::string_view> commands_from_integrity_parse(const std::vector<std::string_view>& input,ErrorCode& err){
    using namespace translate::token;
    Integrity hIntegrity;
    std::vector<std::string_view> commands;
    std::vector<std::pair<uint8_t,std::string_view>> aliases_parameters;
    for(size_t i=0;i<input.size();++i){
        switch(translate_from_txt<Command>(input[i++])){
            default:{
                commands_from_search_process(input[i-1],input[i],hIntegrity,aliases_parameters,err);
                if(err!=ErrorCode::NONE)
                    return {};
                else{
                    commands.push_back(input[i]);
                    break;
                }
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"unknown \"Check\" mode argument",AT_ERROR_ACTION::CONTINUE);
                return {};
            }
        }
    }
    if(commands.empty()){
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Zero args for check mode",AT_ERROR_ACTION::CONTINUE);
        return {};
    }
    return commands;
}