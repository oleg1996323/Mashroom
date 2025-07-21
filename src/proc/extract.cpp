#include <cassert>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <set>
#include <map>
#include <regex>
#include "program/mashroom.h"
#include "proc/extract.h"
#include "compressor.h"
#include <iostream>
#include "proc/extract.h"
#include "def.h"
#include "message.h"
#include "int_pow.h"
#include "data/info.h"
#include "generated/code_tables/eccodes_tables.h"

Extract::ExtractFormat operator|(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs){
    return (Extract::ExtractFormat)((int)lhs|(int)(rhs));
}
Extract::ExtractFormat operator&(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs){
    return (Extract::ExtractFormat)((int)lhs&(int)(rhs));
}
Extract::ExtractFormat& operator|=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs){
    return lhs=lhs|rhs;
}
Extract::ExtractFormat& operator&=(Extract::ExtractFormat& lhs,Extract::ExtractFormat rhs){
    return lhs=lhs&rhs;
}
Extract::ExtractFormat operator^(Extract::ExtractFormat lhs,Extract::ExtractFormat rhs){
    return (Extract::ExtractFormat)((int)lhs^(int)(rhs));
}
Extract::ExtractFormat operator~(Extract::ExtractFormat val){
    return (Extract::ExtractFormat)(~((int)val));
}

namespace fs = std::filesystem;
using namespace std::chrono;
void Extract::__extract__(const fs::path& file, ExtractedData& ref_data,const SublimedDataInfo& positions){
    HGrib1 grib;
    try{
        grib.open_grib(file);
    }
    catch(const std::runtime_error& err){
        std::cerr<<err.what()<<std::endl;
        exit(0);
    }
    
    if(grib.file_size()==0)
        return;
    for(const auto& pos:positions.buf_pos_){
        if(!grib.set_message(pos))
            continue;
        const auto& msg = grib.message();
        if(!msg.has_value())
            throw std::runtime_error("Message undefined");
        if(msg.value().get().message_length()==0)
            grib.next_message();

		//ReturnVal result_date;
        if(stop_token_.stop_requested())
            return;
        GribMsgDataInfo msg_info(msg.value().get().section2().has_value()?msg.value().get().section2().value().get().define_grid():GridInfo{},
                                    msg.value().get().section_1_.date(),
                                    grib.current_message_position(),
                                    grib.current_message_length().value(),
                                    msg.value().get().section_1_.IndicatorOfParameter(),
                                    msg.value().get().section_1_.unit_time_range(),
                                    msg.value().get().section_1_.center(),
                                    msg.value().get().section_1_.table_version());
        // if(props_.center_.has_value() && msg_info.center!=props_.center_)
        //     continue;
        // if(!props_.parameters_.empty() && !props_.parameters_.contains(SearchParamTableVersion{.param_=msg_info.parameter,.t_ver_=msg_info.table_version}))
        //     continue;
        // if(props_.fcst_unit_.has_value() && msg_info.t_unit!=props_.fcst_unit_.value())
        //     continue;
        // if(props_.grid_type_.has_value()){
        //     if(!msg_info.grid_data.has_grid())
        //         continue;
        //     if(props_.grid_type_.value()!=msg_info.grid_data.value().rep_type)
        //         continue;                  
        //     if(msg_info.date>props_.to_date_ || msg_info.date<props_.from_date_){
        //         continue;
        //     }
        //     if(!pos_in_grid(props_.position_.value(),msg_info.grid_data.value()))
        //         continue;
        // }
        if(msg_info.grid_data.has_grid())
            ref_data[CommonDataProperties(msg_info.center,msg_info.table_version,msg_info.t_unit,msg_info.parameter)].emplace_back(
                msg_info.date,msg.value().get().extract_value(value_by_raw(props_.position_.value(),msg_info.grid_data)));
        else continue; //TODO still not accessible getting data without position
    }
    return;
}

using namespace std::string_literals;

#include <format>
#include <chrono>

ErrorCode Extract::__create_dir_for_file__(const fs::path& out_f_name){
    if(!fs::exists(out_f_name.parent_path()))
        if(!fs::create_directories(out_f_name.parent_path())){
            log().record_log(ErrorCodeLog::CANNOT_ACCESS_PATH_X1,"",out_f_name.relative_path().c_str());
            return ErrorCode::INTERNAL_ERROR;
        }
    return ErrorCode::NONE;
}

ErrorCode Extract::__create_file_and_write_header__(std::ofstream& file,const fs::path& out_f_name,const ExtractedData& result){
    {
        auto err = __create_dir_for_file__(out_f_name);
        if(err!=ErrorCode::NONE)
            return err;
    }
    file.open(out_f_name,std::ios::trunc|std::ios::out);
    if(!file.is_open()){
        if(fs::exists(out_f_name) && fs::is_regular_file(out_f_name) && fs::status(out_f_name).permissions()>fs::perms::none){
            log().record_log(ErrorCodeLog::FILE_X1_PERM_DENIED,"",out_f_name.c_str());
            return ErrorCode::INTERNAL_ERROR;
        }   
    }
    else
        std::cout<<"Writing to "<<out_f_name<<std::endl;

    file<<"Mashroom extractor v=0.01\nData formats: "<<center_to_abbr(props_.center_.value())<<"\nSource: https://cds.climate.copernicus.eu/\nDistributor: Oster Industries LLC\n";
    //print header
    //print data to file
    file<<std::left<<std::setw(18)<<"Time"<<"\t";
    for(auto& [cmn_data,values]:result){
        file<<std::left<<std::setw(10)<<parameter_table(
                cmn_data.center_.has_value()?
                cmn_data.center_.value():
                Organization::Undefined,
                cmn_data.table_version_.has_value()?
                cmn_data.table_version_.value():
                0,cmn_data.parameter_.has_value()?
                cmn_data.parameter_.value():0)->name<<'\t';
    }
    file<<std::endl;
    return ErrorCode::NONE;
}

ErrorCode Extract::execute() noexcept{
    if(hProgram){
        path_format = get_dir_file_outp_format();
        auto matched = hProgram->data().match(props_.center_.value(),
                                                props_.fcst_unit_,
                                                props_.parameters_,
                                                TimeInterval{props_.from_date_,props_.to_date_},
                                                props_.grid_type_.value(),
                                                props_.position_.value());
        ExtractedData result;
        for(auto& [path,positions]:matched){   
            if(path.type_!=path::TYPE::FILE || !fs::is_regular_file(path.path_)){
                ErrorPrint::print_error(ErrorCode::X1_IS_NOT_FILE,"",AT_ERROR_ACTION::CONTINUE,path.path_);
                continue;
            }
            if(!fs::exists(path.path_)){
                ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,path.path_);
                continue;
            }
            std::cout<<"Extracting from "<<path<<std::endl;
            std::sort(positions.buf_pos_.begin(),positions.buf_pos_.end());
            if(stop_token_.stop_requested())
                return ErrorCode::INTERRUPTED;
            __extract__(path.path_,result,positions);
            if(stop_token_.stop_requested())
                return ErrorCode::INTERRUPTED;
        }
        utc_tp current_time = utc_tp::max();
        size_t max_length = 0;
        std::vector<decltype(result)::mapped_type*> col_vals_;
        for(auto& [cmn_data,values]:result){
            std::sort(values.begin(),values.end(),std::less());
            if(!values.empty()){
                current_time = std::min(values.front().time_date,current_time);
                col_vals_.push_back(&values);
            }
            max_length = std::max(max_length,values.size());
        }

        std::vector<int> rows;
        rows.resize(col_vals_.size());
        utc_tp file_end_time = t_off_.get_next_tp(current_time);
        std::ofstream out;
        fs::path out_f_name;
        cpp::zip_ns::Compressor cmprs(out_path_,"any.zip");
        for(int row=0;row<max_length;++row){
            if(stop_token_.stop_requested())
                return ErrorCode::INTERRUPTED;
            //if current_time>file_end_time
            current_time = utc_tp::max();
            for(int col=0;col<col_vals_.size();++col)
                if(rows[col]<col_vals_[col]->size())
                    current_time = std::min((*col_vals_.at(col))[rows.at(col)].time_date,current_time);
            if(current_time>=file_end_time || !out.is_open()){
                if(out.is_open()){
                    out.close();
                    if(static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_)&
                    static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::ARCHIVED)){
                        cmprs.add_file(out_path_,out_f_name);              
                    }
                }
                out_f_name/=__generate_directory__(current_time);
                out_f_name/=__generate_name__(output_format_,center_to_abbr(props_.center_.value()),
                grid_to_abbr(props_.grid_type_.value()),current_time);
                {
                    auto err = __create_file_and_write_header__(out,out_f_name,result);
                    if(err!=ErrorCode::NONE)
                        return err;
                }
                file_end_time = t_off_.get_next_tp(current_time);
            }
            out<<std::format("{:%Y/%m/%d %H:%M:%S}",time_point_cast<std::chrono::seconds>(current_time))<<'\t';
            for(int col=0;col<col_vals_.size();++col){
                if(rows[col]<col_vals_[col]->size()){
                    if((*col_vals_[col])[rows[col]].time_date==current_time){
                        out<<std::left<<std::setw(10)<<(*col_vals_[col])[rows[col]].value<<'\t';
                        ++rows[col];
                    }
                    else{
                        out<<std::left<<std::setw(10)<<"NaN"<<'\t';
                    }
                }
                else out<<std::left<<std::setw(10)<<"NaN"<<'\t';
            }
            out<<std::endl;
        }
        if(out.is_open()){
            out.close();
            if(static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_)&
            static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::ARCHIVED)){
                cmprs.add_file(out_path_,out_f_name);              
            }
        }
    }
    return ErrorCode::NONE;
}