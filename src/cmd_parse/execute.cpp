#include "cmd_parse/execute.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include "error_code.h"
#include "err_msg.h"
#include "error_print.h"
#include "cmd_parse/functions.h"

#include "capitalize_parse.h"
#include "check_parse.h"
#include "extract_parse.h"
#include "config_parse.h"
#include "cmd_parse/cmd_translator.h"
#include "help.h"
#include "application.h"
#include <ranges>


void execute(const std::vector<std::string_view>& argv){
    std::cout << "Command-line arguments:" << std::endl;
    if(argv.size()<1)
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Zero arguments",AT_ERROR_ACTION::ABORT,"");

    for (size_t i = 0;i<argv.size();++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }
    MODE mode = MODE::NONE;
    switch(translate_from_txt<translate::token::ModeArgs>(argv.at(0))){
        case translate::token::ModeArgs::EXTRACT:
            mode = MODE::EXTRACT;
            break;
        case translate::token::ModeArgs::CAPITALIZE:
            mode = MODE::CAPITALIZE;
            break;
        case translate::token::ModeArgs::CHECK:
            mode = MODE::CHECK_ALL_IN_PERIOD;
            break;
        case translate::token::ModeArgs::CONFIG:
            mode = MODE::CONFIG;
            break;
        case translate::token::ModeArgs::HELP:
            mode = MODE::HELP;
            for(int i=1;i<argv.size();++i)
                ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"",AT_ERROR_ACTION::CONTINUE,argv.at(i));
            help();
            return;
            break;
        default:
            if(Application::config().has_config_name(argv.at(0))){
                for(int i=1;i<argv.size();++i)
                    ErrorPrint::print_error(ErrorCode::IGNORING_VALUE_X1,"",AT_ERROR_ACTION::CONTINUE,argv.at(i));
                return execute(Application::config().get_user_config(argv.at(0))|std::ranges::views::transform([](auto& str){
                    return std::string_view(str);
                })|std::ranges::to<std::vector<std::string_view>>());
            }
            else
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Undefined mode argument",AT_ERROR_ACTION::ABORT,argv.at(0));
    }
    
    std::vector<std::string_view> args(argv.begin()+1,argv.end());
    {
        switch(mode){
            case MODE::CAPITALIZE:
                capitalize_parse(args);
                break;
            case MODE::EXTRACT:
                extract_parse(args);
                break;
            case MODE::CHECK_ALL_IN_PERIOD:
                check_parse(args);
                break;
            case MODE::CONFIG:
                config_parse(args);
                break;
            case MODE::HELP:
                help();
                break;
            default:
                ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missing mode operation 1st argument",AT_ERROR_ACTION::ABORT);
        }
    }
    //for(int i = 1;i<argv.size();++i){
        //date from for extraction
        //input separated by ':' with first tokens ('h','d','m','y'),integer values
        // else if(strcmp(argv[i],"-coord")==0){
        //     if(mode_extract == DataExtractMode::RECT){
        //         std::cout<<"Conflict between arguments. Already choosen extraction mode by zone-rectangle. Abort"<<std::endl;
        //         exit(1);
        //     }
        //     else if(mode_extract == DataExtractMode::POSITION){
        //         std::cout<<"Ignoring argument: "<<argv[i++]<<std::endl;
        //         continue;
        //     }
        //     else mode_extract=DataExtractMode::POSITION;
        //     ++i;
        //     char* str = strtok(argv[i],":");
        //     coord.lat_ = std::stod(str);
        //     str = strtok(argv[i],":");
        //     coord.lon_ = std::stod(str);
        // }
        //input integer or float value with '.' separation
        
        //input integer or float value with '.' separation

        //input integer or float value with '.' separation
        
        
        
        // else if(strcmp(argv[i],"-send")==0){
        //     ++i;
        //     char* arg = argv[i];
        //     char* tokens;
        //     char* type = strtok_r(arg,":",&tokens);
        //     if(strcmp(type,"dir")==0){
        //         out = strtok_r(NULL,":",&tokens);
        //         if(!std::filesystem::is_directory(out)){
        //             if(!std::filesystem::create_directory(out)){
        //                 std::cout<<out<<" is not a directory. Abort.";
        //                 exit(1);
        //             }
        //         }
        //     }
        //     else if(strcmp(type,"ip")==0 && mode==MODE::EXTRACT){
        //         out = strtok_r(NULL,":",&tokens);
        //     }
        //     else{
        //         std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
        //         exit(1);
        //     }
        // }
        // else{
        //     std::cout<<"Invalid argument: argv["<<argv[i]<<"]"<<std::endl;
        //     exit(1);
        // }
    //}    
    return;
}