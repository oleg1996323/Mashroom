#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "filesystem.h"
#include "proc/index/indexdatafileformat.h"
#include "types/time_interval.h"
#include "API/grib1/include/sections/grid/grid.h"

enum class DIR_TREE_TOKEN {
    HOUR,
    DAY,
    MONTH,
    YEAR,
    LATITUDE,
    LONGITUDE,
    LATLON,
    GRID
};

const std::unordered_map<DIR_TREE_TOKEN,std::unordered_set<std::string>>& token_to_txt() noexcept;
const std::unordered_map<std::string,DIR_TREE_TOKEN>& txt_to_token() noexcept;
std::vector<DIR_TREE_TOKEN> input_to_token_sequence(const std::string&);
bool check_format(std::string_view fmt);
bool check_format(const std::vector<DIR_TREE_TOKEN>& tokens);
std::string format(DIR_TREE_TOKEN token);

std::string generate_filename_format(std::vector<DIR_TREE_TOKEN> tokens) noexcept;

template <typename... ARGS>
fs::path generate_filename(IndexDataFileFormats::token output_format,const std::vector<DIR_TREE_TOKEN>& tokens,ARGS &&...args){
    std::string format_tmp = generate_filename_format(tokens);
    if(!format_tmp.empty()){
        return std::vformat(format_tmp + token_to_extension(output_format), std::make_format_args(args...));
    }
    else throw std::invalid_argument("Invalid filename format");
}

std::string generate_directory_format(const std::vector<DIR_TREE_TOKEN>& tokens) noexcept;
template <typename... ARGS>
fs::path generate_directory(const fs::path& parent_path,const std::vector<DIR_TREE_TOKEN>& tokens,ARGS &&...args){   
    using namespace std::string_literals;
    return parent_path / std::vformat(generate_directory_format(tokens), std::make_format_args(args...));
}

