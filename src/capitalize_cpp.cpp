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
    FormatBinData data;
    data.order_ = order;
    for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(from)){
        if(entry.is_regular_file() && entry.path().has_extension() && 
        (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            std::cout<<entry.path()<<std::endl;
            CapitalizeData cap_data = capitalize(entry.path().c_str(),root.c_str(),hier.c_str(),order.fmt);

            if(cap_data.err==ErrorCodeData::NONE_ERR){
                if(is_correct_date(&cap_data.from)){
                    if(!is_correct_date(&data.data_.from) || get_epoch_time(&cap_data.from)<get_epoch_time(&data.data_.from))
                        data.data_.from=cap_data.from;
                }
                else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Capitalize mode error",AT_ERROR_ACTION::ABORT);

                if(is_correct_date(&cap_data.to)){
                    if(!is_correct_date(&data.data_.to) || get_epoch_time(&cap_data.to)>get_epoch_time(&data.data_.to))
                        data.data_.to=cap_data.to;
                }
                else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Capitalize mode error",AT_ERROR_ACTION::ABORT);
            }
            else ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"Capitalize mode error",AT_ERROR_ACTION::ABORT);
        }
    }
    format::write(root,data);
}