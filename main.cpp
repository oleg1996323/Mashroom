#include <iostream>
#include <dlfcn.h>
#include <memory>
#include <filesystem>
#include "functions.h"

int main(){
    std::filesystem::path path = std::filesystem::current_path();
    for(const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(path)){
        if(entry.is_regular_file())
            if(entry.path().has_extension() && (entry.path().extension()==".grib" || entry.path().extension()==".grb")){
                std::cout<<entry.path()<<std::endl;
                extract_by_date(entry.path(),{1990,01,01},std::string("./new.txt"));
            }
    }
}