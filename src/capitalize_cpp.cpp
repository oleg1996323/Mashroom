#include "capitalize_cpp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "cmd_parse/functions.h"
#include "sys/application.h"
#include "format.h"

using namespace std::string_view_literals;
void capitalize_cpp(const std::filesystem::path& from,const std::filesystem::path& root,const std::string& order="", DataFormat fmt=REF, std::optional<BinaryGribInfoType> bin_info_type={}){
    //std::vector<std::filesystem::directory_entry> entries;
    Capitalize capitalize_obj;
    capitalize_obj.set_output_type(fmt);
    std::string_view order_overriden = order;
    if(order_overriden.empty())
        order_overriden = "ym"sv;
    capitalize_obj.set_output_format(order_overriden);
    capitalize_obj.set_dest_dir(root.c_str());
    for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(from)){
        if(entry.is_regular_file() && entry.path().has_extension() && 
        (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            std::cout<<entry.path()<<std::endl;
            capitalize_obj.set_from_path(from.c_str());
            const GribDataInfo& cap_data = capitalize_obj.execute();
            for(const auto& [common,grib_data]:cap_data.data())
                for(const GribCapitalizeDataInfo& c_d:grib_data){
                    if(c_d.err!=ErrorCodeData::NONE_ERR)
                        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Capitalize mode error",AT_ERROR_ACTION::ABORT,entry.path().c_str(),c_d.buf_pos_);
                }
        }
        else continue;
    }
    auto tmp_res = std::move(capitalize_obj.release_result());
    tmp_res.sublime();
    BinaryGribInfo data(std::move(tmp_res),order);
    data.write(root/"binfmt.bin",BinaryGribInfoType::FULL_DATA);
}