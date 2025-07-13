#include "application.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <ranges>
#include "program/mashroom.h"
#include "cmd_parse/mashroom_parse.h"

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
    if(argc==2 && std::string_view(argv[1],std::strlen(argv[1])+1)=="launch"){
        hProgram = std::make_unique<Mashroom>(Mashroom());
        while(hProgram->read_command(std::cin)){}
    }
    else{
        std::vector<std::string> commands;
        commands.reserve(argc);

        for (int i = 1; i < argc; ++i) {
            std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
            commands.push_back(argv[i]);
        }
        Mashroom::read_command(commands);
    }
    #endif
    return 0;
}