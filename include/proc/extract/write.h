#pragma once
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/log_err.h"
#include "API/grib1/include/properties.h"
#include "proc/extract/extracted_value.h"
#include <thread>
#include "types/time_period.h"
#include "sys/outputdatafileformats.h"
#include "sys/application.h"

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
    auto get = [](auto&& ed) -> const std::decay_t<decltype(ed)>&
    {
        return ed;
    };
    return std::visit(get,result);
}

//"Data formats: "s+center_to_abbr(props.center_.value())+"\nSource: https://cds.climate.copernicus.eu/\nDistributor: Oster Industries LLC\n"
template<typename... ARGS>
std::string get_header(const ExtractedData& result, const SearchProperties& props, std::string_view user_hdr_format) noexcept{
    std::ostringstream stream;
    stream<<"Mashroom extractor v=0.01\n";
    std::string user_header = std::vformat(user_hdr_format,std::make_format_args(user_hdr_format));
    stream<<(user_header.empty()?"":"\n");
    auto write_columns = [&stream](auto&& value){
        if constexpr(std::is_same_v<ExtractedValues<Data_t::METEO,Data_f::GRIB>,std::decay_t<decltype(value)>>){
            stream<<std::left<<std::setw(18)<<"Time"<<"\t";
            for(auto& [cmn_data,values]:value){
                stream<<std::left<<std::setw(10)<<parameter_table(
                        cmn_data.center_.has_value()?
                        cmn_data.center_.value():
                        Organization::Undefined,
                        cmn_data.table_version_.has_value()?
                        cmn_data.table_version_.value():
                        0,cmn_data.parameter_.has_value()?
                        cmn_data.parameter_.value():0)->name<<'\t';
            }
            stream<<std::endl;
        }
        else static_assert(false,"Undefined text format header");
    };
    std::visit(write_columns,result);
    return stream.str();
}

ErrorCode write_txt_file(const std::stop_token& stop_token,
                        ExtractedData& result,
                        const SearchProperties& props,
                        const TimePeriod& t_off,
                        const fs::path& out_path,
                        const std::string& dirname_format,
                        const std::string& filename_format,
                        OutputDataFileFormats output_format,
                        std::unordered_set<fs::path>& paths) noexcept;