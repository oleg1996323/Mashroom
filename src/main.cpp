#include "application.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <ranges>
#include "cmd_parse/execute.h"

static int progress_line = 6;

int main(int argc, char* argv[]){
    #ifdef PERMANENT_GET 
        while(int ch =std::getchar()){
            std::cout<<ch<<std::endl;
        }
        //will be added
    #else
    std::cout << "Command-line arguments:" << std::endl;
    if(argc<2){
        std::cout<<"Invalid args. Abort."<<std::endl;
        exit(1);
    }

    std::vector<std::string_view> commands;
    commands.reserve(argc);

    for (int i = 1; i < argc; ++i) {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
        commands.push_back(argv[i]);
    }

    execute(commands);
    #endif
    return 0;
}