#pragma once

#ifdef __cplusplus
#include <filesystem>
#include <string>
#include <string_view>
namespace fs = std::filesystem;
bool change_directory(const char* dir);
bool change_directory(const fs::path& dir);
bool change_directory(const std::string& dir);
bool change_directory(std::string_view dir);
#else
extern bool change_directory(const char* dir);
#endif

