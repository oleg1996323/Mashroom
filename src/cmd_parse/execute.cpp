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
#include <ranges>



void set_config_parse(std::string_view input){

}

void execute(std::vector<std::string_view> argv){
    std::cout << "Command-line arguments:" << std::endl;
    if(argv.size()<1){
        std::cout<<"Invalid args. Abort."<<std::endl;
        exit(1);
    }

    for (size_t i = 0;i<argv.size();++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }


    std::filesystem::path path;
    std::filesystem::path out;
    MODE mode = MODE::NONE;
    
    if(MODE::NONE==mode){
        if(argv[1]=="-ext")
            mode = MODE::EXTRACT;
        else if(argv[1]=="-cap")
            mode = MODE::CAPITALIZE;
        //enable the checking mode of full full package of downloaded files
        else if(argv[1]=="-check")
            mode = MODE::CHECK_ALL_IN_PERIOD;
        else
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missing mode operation 1st argument.",AT_ERROR_ACTION::ABORT);
    }
    else
        ErrorPrint::print_error(ErrorCode::INCOR_ARG_X1_ALREADY_CHOOSEN_MODE_X2,"",AT_ERROR_ACTION::ABORT,argv[1],get_string_mode(mode));
    std::ranges::subrange<std::vector<std::string_view>::iterator> args(argv.begin()+1,argv.end());
    switch(mode){
        case MODE::CAPITALIZE:
            capitalize_parse({args.begin(),args.end()});
        case MODE::EXTRACT:
            extract_parse({args.begin(),args.end()});
        case MODE::CHECK_ALL_IN_PERIOD:
            check_parse({args.begin(),args.end()});
        default:
            ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"Missing mode operation 1st argument.",AT_ERROR_ACTION::ABORT);
    }
    for(int i = 2;i<argv.size();++i){
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
    }    
    return;
}