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
#include <iostream>
#include "proc/extract.h"
#include "message.h"
#include "int_pow.h"
#include "generated/code_tables/eccodes_tables.h"
#include "API/common/error_data.h"
#include "API/common/error_data_print.h"
#include "proc/extract/gen.h"
#include "proc/extract/write.h"
#include "compressor.h"
#include "sys/error_exception.h"

namespace fs = std::filesystem;
using namespace std::chrono;

ErrorCode Extract::__write_file__(ExtractedData& result,OutputDataFileFormats FORMAT) const{
    ErrorCode err = ErrorCode::NONE;
    std::unordered_set<fs::path> paths;
    try{
        switch(FORMAT&~OutputDataFileFormats::ARCHIVED){
            case OutputDataFileFormats::DEFAULT:
            case OutputDataFileFormats::TXT_F:{
                paths.merge(procedures::extract::write_txt_file(stop_token_,result,props_,t_off_,out_path_));
                break;
            }
            case OutputDataFileFormats::BIN_F:{
                paths.merge(procedures::extract::write_bin_file(stop_token_,result,props_,t_off_,out_path_));
                break;
            }
            case OutputDataFileFormats::JSON_F:{
                paths.merge(procedures::extract::write_json_file(stop_token_,result,props_,t_off_,out_path_));
                break;
            }
            default:{
                paths.merge(procedures::extract::write_txt_file(stop_token_,result,props_,t_off_,out_path_));
                break;
            }
        }
    }
    catch(const ErrorException& err){
        return err.error();
    }
    if((FORMAT&OutputDataFileFormats::ARCHIVED)!=0){
        try{
            auto cmprs = cpp::zip_ns::Compressor::create_archive(out_path_,std::to_string(utc_tp::clock::now().time_since_epoch().count()));
            for(auto& path:paths){
                if(!cmprs.add_file(out_path_,path))
                    throw ErrorException(ErrorCode::INTERNAL_ERROR,std::string_view("archive creation failure"));
                else continue;
            }
        }
        catch(const ErrorException& err){
            return err.error();
        }
    }
    return ErrorCode::NONE;
}

ErrorCode Extract::__extract__(const fs::path& file, ExtractedData& ref_data){
    HGrib1 grib;
    
    if(API::ErrorData::Code<API::GRIB1>::value err_data = grib.open_grib(file);err_data!=API::ErrorData::Code<API::GRIB1>::NONE_ERR)
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,API::ErrorDataPrint::message<API::GRIB1>(err_data,"",file.string()),AT_ERROR_ACTION::CONTINUE);
    
    if(grib.file_size()==0)
        return ErrorPrint::print_error(ErrorCode::DATA_NOT_FOUND,"Message undefined",AT_ERROR_ACTION::CONTINUE);
    do{
        const auto& msg = grib.message();
        if(!msg.has_value())
            return ErrorPrint::print_error(ErrorCode::DATA_NOT_FOUND,"Message undefined",AT_ERROR_ACTION::CONTINUE);
        if(msg->get().message_length()==0)
            grib.next_message();

		//ReturnVal result_date;
        if(stop_token_.stop_requested())
            return ErrorCode::NONE;
        GribMsgDataInfo msg_info(msg->get().section2().has_value()?msg->get().section2()->get().define_grid():GridInfo{},
                                    msg->get().section_1_.reference_time(),
                                    grib.current_message_position(),
                                    grib.current_message_length().value(),
                                    msg->get().section_1_.parameter_number(),
                                    msg->get().section_1_.time_forecast(),
                                    msg->get().section_1_.center(),
                                    msg->get().section_1_.table_version(),
                                    msg->get().section_1_.level_data(),
                                    msg->get().err_);
        if(props_.center_.has_value() && msg_info.center!=props_.center_)
            continue;
        if(!props_.parameters_.empty() && !props_.parameters_.contains(SearchParamTableVersion{.param_=msg_info.parameter,.t_ver_=msg_info.table_version}))
            continue;
        if(props_.fcst_unit_.has_value() && msg_info.t_unit!=props_.fcst_unit_.value())
            continue;
        if(props_.grid_type_.has_value()){
            if(!msg_info.grid_data.has_grid())
                continue;
            if(props_.grid_type_.value()!=msg_info.grid_data.type())
                continue;                  
            if(msg_info.date>props_.to_date_ || msg_info.date<props_.from_date_){
                continue;
            }
            if(!pos_in_grid(props_.position_.value(),msg_info.grid_data))
                continue;
        }
        if(msg_info.grid_data.has_grid())
            procedures::extract::get_result(ref_data)[
                Grib1CommonDataProperties(msg_info.center,
                    msg_info.table_version,
                    msg_info.parameter)]
                    .emplace_back(
                msg_info.date,msg->get().extract_value(value_by_raw(props_.position_.value(),msg_info.grid_data)));
        else continue; //TODO still not accessible getting data without position
    }while(grib.next_message());
    return ErrorCode::NONE;
}

template<>
ErrorCode Extract::__extract__<Data_t::TIME_SERIES,Data_f::GRIB_v1>(const fs::path &file, ExtractedData &ref_data, const std::vector<ptrdiff_t>& positions){
    HGrib1 grib;
    try{
        grib.open_grib(file);
    }
    catch(const std::runtime_error& err){
        std::cerr<<err.what()<<std::endl;
        exit(0);
    }
    
    if(grib.file_size()==0)
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,API::ErrorDataPrint::message<API::GRIB1>(API::ErrorData::Code<API::GRIB1>::DATA_EMPTY_X1,"",file.string()),AT_ERROR_ACTION::CONTINUE);
    for(const auto& pos:positions){
        if(pos<0 || !grib.set_message(pos))
            continue;
        const auto& msg = grib.message();
        if(!msg.has_value())
            throw std::runtime_error("Message undefined");
        if(msg->get().message_length()==0)
            grib.next_message();

		//ReturnVal result_date;
        if(stop_token_.stop_requested())
            return ErrorCode::INTERRUPTED;
        GribMsgDataInfo msg_info(msg->get().section2().has_value()?msg->get().section2()->get().define_grid():GridInfo{},
                                    msg->get().section_1_.reference_time(),
                                    grib.current_message_position(),
                                    grib.current_message_length().value(),
                                    msg->get().section_1_.parameter_number(),
                                    msg->get().section_1_.time_forecast(),
                                    msg->get().section_1_.center(),
                                    msg->get().section_1_.table_version(),
                                    msg->get().section_1_.level_data(),
                                    msg->get().err_);
        if(msg_info.grid_data.has_grid())
            procedures::extract::get_result(ref_data)[
                Grib1CommonDataProperties(
                    msg_info.center,
                    msg_info.table_version,
                    msg_info.parameter)].emplace_back(
                msg_info.date,msg->get().extract_value(value_by_raw(props_.position_.value(),msg_info.grid_data)));
        else continue; //TODO still not accessible getting data without position
    }
    return ErrorCode::NONE;
}

using namespace std::string_literals;

#include <format>
#include <chrono>

ErrorCode Extract::execute() noexcept{
    ExtractedData result;
    if(in_path_.empty()){
        auto matched = Mashroom::instance().data().match_data<Data_t::TIME_SERIES,Data_f::GRIB_v1>(props_.center_.value(),
                                                props_.fcst_unit_,
                                                props_.level_,
                                                props_.parameters_,
                                                TimeInterval(props_.from_date_,props_.to_date_),
                                                props_.grid_type_.value(),
                                                props_.position_.value());
        
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
            std::sort(positions.begin(),positions.end());
            if(stop_token_.stop_requested())
                return ErrorCode::INTERRUPTED;
            __extract__(fs::path(path.path_),result,positions);
            if(stop_token_.stop_requested())
                return ErrorCode::INTERRUPTED;
        }
    }
    else{
        for(const auto& path:in_path_){
            if(Mashroom::instance().data().paths<Data_t::TIME_SERIES,Data_f::GRIB_v1>().contains(path))
                Mashroom::instance().data().match(
                                                path,
                                                props_.center_.value(),
                                                props_.fcst_unit_,
                                                props_.level_,
                                                props_.parameters_,
                                                TimeInterval(props_.from_date_,props_.to_date_),
                                                props_.grid_type_.value(),
                                                props_.position_.value());
            __extract__(path.path_,result);
        }
    }
    if(procedures::extract::get_result(result).empty())
        return ErrorCode::NONE;
        
    __write_file__(result,output_format_);
    return ErrorCode::NONE;
}

ErrorCode Extract::__extract__(const fs::path &file, ExtractedData &ref_data, const std::vector<ptrdiff_t>& positions){
    auto auto_unpack = [this,&file,&ref_data,&positions](auto& data){
        using T = std::decay_t<decltype(data)>;
        if constexpr(std::is_same_v<T,std::monostate>)
            return ErrorCode::NONE;
        else {
            auto internal_auto_unpack=[this,&file,&ref_data,&positions]<Data_t TYPE,Data_f FORMAT>(ExtractedValues<TYPE,FORMAT>& internal_data){
                return __extract__<TYPE,FORMAT>(file,ref_data,positions);
            };
            return internal_auto_unpack(data);
        }
    };
    return std::visit(auto_unpack,ref_data);
}