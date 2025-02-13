#pragma once
#include <filesystem>
#include <string>
#include <fstream>
#include <memory>
#include <ranges>
#include <regex>
#include <chrono>

#ifdef __cplusplus
extern "C"{
    #include "PDSdate.h"
    #include "extract.h"
}
#endif

bool parse_dir_name_match(const std::filesystem::path& path,char type){
    if(!std::filesystem::is_directory(path))
        throw std::runtime_error("Error at extraction");
}

struct DirList{
    std::filesystem::directory_iterator cur_dir;
    std::filesystem::directory_entry cur_iter; 
    std::unique_ptr<DirList> ex;
    char type;

    void next(){
        
    }
};

void extract_cpp(const std::filesystem::path& root_path,
                std::chrono::time_point<std::chrono::system_clock> from, 
                std::chrono::time_point<std::chrono::system_clock> to){
    std::string fmt;
    {
        std::ifstream fmt_f("format.bin",std::ios::binary);
        fmt_f.seekg(0,std::ios::end);
        unsigned end=fmt_f.tellg();
        fmt_f.seekg(0);
        fmt.resize(end);
        fmt_f.read(fmt.data(),end);
    }
    using namespace std::chrono;
    time_t from_t = system_clock::to_time_t(from);
    time_t to_t = system_clock::to_time_t(to);
    tm* local_from_t = localtime(&from_t);
    tm* local_to_t = localtime(&to_t);
    DirList root;
    root.cur_dir = std::filesystem::directory_iterator(root_path);
    DirList* cur_dir = &root;
    size_t count = 0;
    
    for(const std::filesystem::directory_entry& entry:cur_dir->cur_dir){
        if(count<fmt.size()){
            switch(fmt.at(count)){
                case 'Y':{
                    std::regex reg("R(^19[0-9][0-9]|20[0-9][0-9]$)");
                    std::regex_match(entry.path().string(),reg);
                    if(std::regex_match(entry.path().string(),reg) &&
                        local_from_t->tm_year<=std::stoi(entry.path().string()) &&
                        local_to_t->tm_year>=std::stoi(entry.path().string())){
                        cur_dir->cur_iter=
                    }   
                    break;
                }
                case 'M':{
                    std::regex reg("R(^([1-9]|0[1-9]|1[0-2])$)");
                    if(std::regex_match(entry.path().string(),reg))
                        std::stoi(entry.path().string());
                    break;
                }
                case 'D':

                case 'H':

                case 'L':

                case 'O':

                case 'C':

                default:
            }
            ++count;
        }
    }
}