#include "cap.h"
#include <iostream>
#include <fstream>
#include <vector>

std::string get_fmt(const OrderItems& order){
    std::vector<int OrderItems::*> items{
                                        &OrderItems::day,
                                        &OrderItems::hour,
                                        &OrderItems::lat,
                                        &OrderItems::lon,
                                        &OrderItems::month,
                                        &OrderItems::year};
    int max = std::max({order.day,order.hour,order.lat,order.lon,order.month,order.year});
    int current_order = 0;
    std::string res;
    while(current_order<=max){
        if(order.day==current_order)
            res.push_back('D');
        else if(order.hour==current_order)
            res.push_back('H');
        else if(order.month==current_order)
            res.push_back('M');
        else if(order.year==current_order)
            res.push_back('Y');
        else if(order.lat==current_order)
            if(order.lat==order.lon)
                res.push_back('C');
            else res.push_back('L');
        else if(order.lon==current_order)
            res.push_back('O');
        ++current_order;
    }
    return res.c_str();
}

void cap(const std::filesystem::path& from,const std::filesystem::path& root,const OrderItems& order){
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
    std::string hier = get_fmt(order);
    for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(from)){
        if(entry.is_regular_file() && entry.path().has_extension() && 
        (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            std::cout<<entry.path()<<std::endl;
            capitalize(entry.path().c_str(),root.c_str(),hier.c_str(),order.fmt);
        }
    }
}