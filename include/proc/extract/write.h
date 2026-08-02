#pragma once
#include "sys/error.h"
#include "grib1/properties.h"
#include "proc/extract/extracted_data.h"
#include <thread>
#include "OsterLib/types/time_period.h"
#include "sys/outputdatafileformats.h"
#include "sys/application.h"
#include "OsterLib/serialization.h"
#include "OsterLib/contexted_error.h"
#include <expected>

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
std::unordered_set<std::filesystem::path>
        write_txt_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const DateTimeDiff& t_off,
                        const std::filesystem::path& out_path);

std::unordered_set<std::filesystem::path> 
        write_json_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const DateTimeDiff& t_off,
                        const std::filesystem::path& out_path);

std::unordered_set<std::filesystem::path> 
        write_bin_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const DateTimeDiff& t_off,
                        const std::filesystem::path& out_path);
}