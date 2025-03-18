#include "capitalize_cpp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "cmd_parse/functions.h"

void capitalize_cpp(const std::filesystem::path& from,const std::filesystem::path& root,const OrderItems& order){
    //std::vector<std::filesystem::directory_entry> entries;
    if(!std::filesystem::is_directory(from)){
        std::cout<<from<<" is not a directory. Abort."<<std::endl;
        exit(1);
    }
    if(!std::filesystem::is_directory(root)){
        if(!std::filesystem::create_directory(root))
            std::cout<<root<<" is not a directory and unable to create directory. Abort."<<std::endl;
            exit(1);
    }
    std::string hier = functions::capitalize::get_txt_order(order);
    for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(from)){
        if(entry.is_regular_file() && entry.path().has_extension() && 
        (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            std::cout<<entry.path()<<std::endl;
            capitalize(entry.path().c_str(),root.c_str(),hier.c_str(),order.fmt);
        }
    }
}