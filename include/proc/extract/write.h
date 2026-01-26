#pragma once
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/log_err.h"
#include "API/grib1/include/properties.h"
#include "proc/extract/extracted_data.h"
#include <thread>
#include "types/time_period.h"
#include "sys/outputdatafileformats.h"
#include "sys/application.h"
#include "serialization.h"

namespace procedures::extract{

inline auto get_columns(ExtractedData& result){
    auto define_cols_t = [](auto& ed) -> std::vector<const typename std::decay_t<decltype(ed)>::mapped_type*>
    {
        return std::vector<const typename std::decay_t<decltype(ed)>::mapped_type*>();
    };
    return std::visit(define_cols_t,result);
}

inline auto& get_result(ExtractedData& result){
    auto get = [](auto& ed) -> std::decay_t<decltype(ed)>&
    {
        return ed;
    };
    return std::visit(get,result);
}

inline const auto& get_result(const ExtractedData& result){
    auto get = [](const auto& ed) -> const std::decay_t<decltype(ed)>&
    {
        return ed;
    };
    return std::visit(get,result);
}

std::unordered_set<fs::path> write_txt_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const DateTimeDiff& t_off,
                        const fs::path& out_path);

std::unordered_set<fs::path> write_json_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const DateTimeDiff& t_off,
                        const fs::path& out_path);

std::unordered_set<fs::path> write_bin_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const DateTimeDiff& t_off,
                        const fs::path& out_path);
}