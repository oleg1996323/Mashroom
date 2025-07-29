#pragma once
#include <filesystem>
#include <string>
#include <string_view>
namespace fs = std::filesystem;
bool change_directory(const char* dir);
bool change_directory(const fs::path& dir);
bool change_directory(const std::string& dir);
bool change_directory(std::string_view dir);

inline bool directory_accessible(const fs::path& path) noexcept{
    if(!fs::exists(path)){
        if(!fs::create_directories(path))
            return false;
        else return true;
    }
    else return true;
}

namespace std::filesystem{
    inline std::string separator(){
        #ifdef _WIN32
            return "\\";
        #else 
            return "/";
        #endif
    }
}