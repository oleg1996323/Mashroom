#pragma once
#include <stdio.h>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include "types/data_info.h"
#include "message.h"

namespace fs = std::filesystem;
using namespace std::string_literals;
static constexpr const char* miss_files_filename = "missing_files.txt";
static constexpr const char* errorness_files_filename = "corrupted_files.txt";
class Check{
    public:
    struct Properties{
        std::optional<CommonDataProperties> common_;
        std::optional<long long> from_date_;
        std::optional<long long> to_date_;
        std::optional<RepresentationType> grid_type_;
        std::optional<Coord> position_;
    };
    private:
    Properties props_;
    GribDataInfo result;
    std::string_view from_file_;
    int cpus = 1;
    public:
    struct Properties{
        std::optional<CommonDataProperties> common_;
        std::optional<long long> from_date_;
        std::optional<long long> to_date_;
        std::optional<RepresentationType> grid_type_;
        std::optional<Coord> position_;
    };

    static bool check_format(std::string_view fmt);
    const GribDataInfo& execute();
    void set_from_path(std::string_view from_file){
        if(!fs::exists(from_file))
            throw std::invalid_argument("File doesn't exists "s + from_file.data());
        from_file_=from_file;
    }
    void set_properties(std::optional<CommonDataProperties> common = {},
                        std::optional<long long> from_date = {},
                        std::optional<long long> to_date = {},
                        std::optional<RepresentationType> grid_type = {},
                        std::optional<Coord> position = {}){
        props_ = Properties({common,from_date,to_date,grid_type,position});
    }
    void set_using_processor_cores(int cores){
        if(cores>0 && cores<std::thread::hardware_concurrency())
            cpus=cores;
        else cpus = 1;
    }
    const GribDataInfo& get_result() const{
        return result;
    }
    void clear_result(){
        result.info_.clear();
        result.err = ErrorCodeData::NONE_ERR;
    }
    GribDataInfo&& release_result() noexcept{
        GribDataInfo res(std::move(result));
        return res;
    }
};