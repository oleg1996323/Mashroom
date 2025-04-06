#include "capitalize_cpp.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "cmd_parse/functions.h"
#include "format.h"

void capitalize_cpp(const std::filesystem::path& from,const std::filesystem::path& root,const OrderItems& order){
    //std::vector<std::filesystem::directory_entry> entries;
    if(!std::filesystem::is_directory(from)){
        std::cout<<from<<" is not a directory. Abort."<<std::endl;
        exit(1);
    }
    if(!std::filesystem::is_directory(root)){
        if(!std::filesystem::create_directory(root))
            std::cout<<root<<" is not a directory and unable to create directory. Abort."<<std::endl;
            exit(1);
    }
    std::string hier = functions::capitalize::get_txt_order(order);
    if(hier.empty())
        hier = "ym";
    if(order.fmt==DataFormat::NONE)
        ErrorPrint::print_error(ErrorCode::COMMAND_INPUT_X1_ERROR,"not defined capitalize format.",AT_ERROR_ACTION::ABORT);
    FormatBinData data;
    data.order_ = functions::capitalize::get_order_txt(hier);
    data.order_.fmt = order.fmt;
    for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(from)){
        if(entry.is_regular_file() && entry.path().has_extension() && 
        (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            std::cout<<entry.path()<<std::endl;
            std::vector<GribDataInfo> cap_data = capitalize(entry.path().c_str(),root.c_str(),hier.c_str(),order.fmt);
            for(const GribDataInfo& c_d:cap_data){
                if(c_d.err==ErrorCodeData::NONE_ERR){
                    if(is_correct_date(&c_d.from)){
                        if(!is_correct_date(&data.data_.from) || get_epoch_time(&c_d.from)<get_epoch_time(&data.data_.from))
                            data.data_.from=c_d.from;
                    }
                    else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Capitalize mode error",AT_ERROR_ACTION::ABORT);

                    if(is_correct_date(&c_d.to)){
                        if(!is_correct_date(&data.data_.to) || get_epoch_time(&c_d.to)>get_epoch_time(&data.data_.to))
                            data.data_.to=c_d.to;
                    }
                    else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Capitalize mode error",AT_ERROR_ACTION::ABORT);

                    if(!is_correct_rect(&c_d.grid_data.bound))
                        ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Capitalize mode error",AT_ERROR_ACTION::ABORT);
                    else data.data_.grid_data=c_d.grid_data;
                }
                else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Capitalize mode error",AT_ERROR_ACTION::ABORT);
            }
        }
    }
    format::write(root,data);
}