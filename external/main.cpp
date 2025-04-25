#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <table_0.h>
#include <string>
#include <string.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]){
    
    std::fstream orgs;
    orgs.open(fs::path(ROOT_DIR)/"organizations.dat",std::ios::trunc|std::ios::out);
    std::cout<<"Trying to open "<<fs::path(ROOT_DIR)/"organizations.dat"<<std::endl;
    if(!orgs.is_open())
        return 0;
    for(int i=0;i<256;++i){
        const char* tmp = center_to_abbr((Organization)i);
        if(strlen(tmp)>0)
            orgs<<i<<" "<<tmp<<" "<<center_to_text((Organization)i)<<std::endl;
    }
    orgs.close();
    return 0;
}