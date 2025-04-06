#pragma once
#include <stdio.h>
#include "types/data_info.h"

#ifdef __cplusplus
#include <vector>
#endif

STRUCT_BEG(ValidCapitalizeFmt)
{
    bool COORD;
    bool YEAR;
    bool MONTH;
    bool DAY;
    bool HOUR;
}
STRUCT_END(ValidCapitalizeFmt)

#ifdef __cplusplus
bool check(char ch, const ValidCapitalizeFmt& valid);
#else
extern bool check(char ch, struct ValidCapitalizeFmt* valid);
#endif

constexpr size_t s = sizeof(GridData);
constexpr size_t s_1 = sizeof(Date);

#ifndef __cplusplus
extern GribDataInfo capitalize(const char* in,
    const char* root_cap_dir_name,
    const char* fmt_cap,
    enum DATA_FORMAT output_type);
#else
#include <filesystem>
namespace fs = std::filesystem;
extern GribDataInfo capitalize(const char* in,
    const fs::path& root_cap_dir_name,
    const std::string& fmt_cap,
    enum DATA_FORMAT output_type);
#endif