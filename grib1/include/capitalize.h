#pragma once
#include <stdio.h>
#include "def.h"
#include "types/date.h"
#include "types/grid.h"
#include "aux_code/def.h"
#include "code_tables/table_0.h"

#ifdef __cplusplus
#include <vector>
#endif

STRUCT_BEG(CapitalizeData)
{
    GridData grid_data;
    Date from;
    Date to;
    #ifdef __cplusplus
    std::vector<uint8_t> parameters;
    #endif
    Organization center;
    uint8_t subcenter;
    #ifndef __cplusplus
    uint8_t* parameters;
    #endif
    ErrorCodeData err DEF_STRUCT_VAL(NONE_ERR)

    #ifdef __cplusplus
    bool contains_param(uint8_t param) const{
        return std::find(parameters.begin(),parameters.end(),param)!=parameters.end();
    }
    #endif
}
STRUCT_END(CapitalizeData)

constexpr size_t s = sizeof(GridData);
constexpr size_t s_1 = sizeof(Date);

#ifndef __cplusplus
#define CapitalizeData(...) ((CapitalizeData){.from = Date(), .to = Date(),.grid_data = GridData(),.sections=0,.err = NONE_ERR})
#endif

#ifndef __cplusplus
extern CapitalizeData* capitalize(const char* in,
    const char* root_cap_dir_name,
    const char* fmt_cap,
    enum DATA_FORMAT output_type);
#else
#include <filesystem>
namespace fs = std::filesystem;
extern std::vector<CapitalizeData> capitalize(const char* in,
    const fs::path& root_cap_dir_name,
    const std::string& fmt_cap,
    enum DATA_FORMAT output_type);
#endif