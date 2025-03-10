#pragma once
#include "cmd_parse/check_parse.h"
#include <thread>
#include <iostream>
#include <cmd_parse/functions.h>
#include <filesystem>
#include <set>
#include <vector>
#include <future>
#include <chrono>
#include <ranges>
#include <fstream>
#include <algorithm>
#include <termios.h>
#include "application.h"
#include "cmd_parse/aux/process_bar.h"
#include "sys/error_print.h"
#include "cmd_parse/cmd_translator.h"
#include "sys/log_err.h"
#ifdef __cplusplus
    extern "C"{
        #include "contains.h"
    }
#endif

static constexpr const char* miss_files_filename = "missing_files.txt";
static constexpr const char* errorness_files_filename = "corrupted_files.txt";

using namespace std::chrono_literals;
namespace fs = std::filesystem;

template<class Clock, class Duration> 
struct std::incrementable_traits<std::chrono::time_point<Clock, Duration>> { 
  using difference_type = typename Duration::rep;
};

struct ErrorFiles{
    fs::path name;
    ErrorCodeData code;
};

struct ProcessResult{
    std::set<std::chrono::sys_days> found;
    std::vector<ErrorFiles> err_files;
};

ProcessResult process_core(std::ranges::random_access_range auto&& entries, std::mutex* mute_at_print = nullptr);


bool missing_files(fs::path in_dir,fs::path out_dir, unsigned int cpus=std::thread::hardware_concurrency(), std::chrono::year_month_day from=1d/1/1991, 
std::chrono::year_month_day to = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));