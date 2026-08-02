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
#include "grib1/message.h"
#include "OsterLib/int_pow.h"
#include "proc/extract/gen.h"
#include "proc/extract/write.h"
#include "OsterLib/compressor.h"
#include "sys/error.h"
#include "grib1/message.h"

namespace fs = std::filesystem;
using namespace std::chrono;
using namespace osterlib;

ContextedError Extract::__write_file__(ExtractedData& result,OutputDataFileFormats FORMAT) const{
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
    catch(const ContextedError& err){
        return err;
    }
    if((FORMAT&OutputDataFileFormats::ARCHIVED)!=0){
        try{
            auto cmprs = cpp::zip_ns::Compressor::create_archive(out_path_,std::to_string(utc_tp::clock::now().time_since_epoch().count()));
            for(auto& path:paths){
                if(!cmprs.add_file(out_path_,path)){
                    ContextedError ctx_err(mashroom::errc::internal_error,"archive creation failure");
                    throw ctx_err.with_field("procedure","extract")
                    .with_field("file",path.c_str());
                }
                else continue;
            }
        }
        catch(const ContextedError& err){
            return err;
        }
    }
    return {};
}

ExtractedData Extract::__extract__(const fs::path& file, osterlib::ContextedError& err){
    api::HGrib1 grib;
    ExtractedData result;
    if(auto err_data = grib.open_grib(file);err_data.code()){
        std::cout<<err_data.what();
        err = err_data.code();
        return ExtractedData();
    }
    
    if(grib.file_size()==0){
        err.error(api::errc<API_T::GRIB1>::data_empty);
        err.with_field("procedure","extract").with_field("file",file.string());
        return ExtractedData();
    }
    do{
        const auto& msg = grib.message();
        if(!msg.has_value()){
            err.error(api::errc<API_T::GRIB1>::data_empty);
            err.with_field("procedure","extract").with_field("file",file.string());
            return ExtractedData();
        }
        if(msg->get().message_length()==0)
            grib.next_message();

		//ReturnVal result_date;
        if(stop_token_.stop_requested()){
            err.error(mashroom::errc::interrupted);
            err.with_field("procedure","extract");
            ///@todo log
            return {};
        }
        data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> msg_info(msg->get().section2().has_value()?msg->get().section2()->get().define_grid():GridInfo{},
                                    msg->get().section_1_.reference_time(),
                                    grib.current_message_position(),
                                    grib.current_message_length().value(),
                                    msg->get().section_1_.parameter_number(),
                                    msg->get().section_1_.time_forecast(),
                                    msg->get().section_1_.center(),
                                    msg->get().section_1_.table_version(),
                                    msg->get().section_1_.level_data(),
                                    msg->get().err_.code());
        if(props_.center_.has_value() && msg_info.center!=props_.center_)
            continue;
        if(!props_.parameters_.empty() && !props_.parameters_.contains(SearchParamTableVersion{.param_=msg_info.parameter,.t_ver_=msg_info.table_version}))
            continue;
        if(props_.fcst_unit_.has_value() && msg_info.t_unit!=props_.fcst_unit_.value())
            continue;
        if(props_.grid_type_.has_value()){
            if(!msg_info.grid_data ||
                !msg_info.grid_data->has_grid())
                continue;
            if(props_.grid_type_.value()!=msg_info.grid_data->type())
                continue;                  
            if((props_.to_date_.has_value() &&
                msg_info.date>*props_.to_date_) ||
                (props_.from_date_.has_value() &&
                msg_info.date<*props_.from_date_)){
                continue;
            }
            if(!pos_in_grid(props_.position_.value(),*msg_info.grid_data))
                continue;
        }
        if(msg_info.grid_data && msg_info.grid_data->has_grid())
            procedures::extract::get_result(result)[
                Grib1CommonDataProperties(msg_info.center,
                    msg_info.table_version,
                    msg_info.parameter)]
                    .emplace_back(
                msg_info.date,msg->get().extract_value(
                        value_by_raw(
                            props_.position_.value(),
                            *msg_info.grid_data)));
        else continue; //TODO still not accessible getting data without position
    }while(grib.next_message());
    return result;
}

template<>
ExtractedData Extract::__extract_spec__<Data_t::TIME_SERIES,Data_f::GRIB_v1>(
        const fs::path &file,
        const std::vector<MessagePositionSizeInfo>& positions,
        osterlib::ContextedError& err){
    api::HGrib1 grib;
    ExtractedData result;
    try{
        grib.open_grib(file);
    }
    catch(const std::runtime_error& err){
        std::cerr<<err.what()<<std::endl;
        exit(0);
    }
    
    if(grib.file_size()==0){
        err.error(api::errc<API_T::GRIB1>::data_empty);
        err.with_field("procedure","extract").with_field("file",file.string());
        return {};
    }
    for(const auto& pos:positions){
        if(pos.begin_<0 || !grib.set_message(pos.begin_))
            continue;
        const auto& msg = grib.message();
        if(!msg.has_value()){
            err.error(mashroom::errc::data_not_found);
            err.with_field("procedure","extract");
            return {};
        }
        if(msg->get().message_length()==0)
            grib.next_message();

		//ReturnVal result_date;
        if(stop_token_.stop_requested()){
            err.error(mashroom::errc::interrupted);
            err.with_field("procedure","extract");
            ///@todo log
            return {};
        }
        data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> msg_info(
            msg->get().section2().has_value()?msg->get().section2()->get().define_grid():GridInfo{},
            msg->get().section_1_.reference_time(),
            grib.current_message_position(),
            grib.current_message_length().value(),
            msg->get().section_1_.parameter_number(),
            msg->get().section_1_.time_forecast(),
            msg->get().section_1_.center(),
            msg->get().section_1_.table_version(),
            msg->get().section_1_.level_data(),
            msg->get().err_.code());
        
        if(msg_info.grid_data && msg_info.grid_data->has_grid()){
            auto add_value = [this,&msg_info,&msg]<Data_t TYPE,Data_f FORMAT>(ExtractedValues<TYPE, FORMAT>& val){
                if constexpr(TYPE == Data_t::TIME_SERIES && FORMAT == Data_f::GRIB_v1){
                    using namespace procedures::extract::details;
                    CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1> cmn;
                    cmn.center_=msg_info.center;
                    cmn.parameter_=msg_info.parameter;
                    cmn.table_version_=msg_info.table_version;
                    AdditionalExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1> add;
                    add.fcst_=msg_info.t_unit;
                    add.grid_=msg_info.grid_data->type();
                    val[ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>(cmn,add)].emplace_back(
                        msg_info.date,msg->get().extract_value(value_by_raw(props_.position_.value(),*msg_info.grid_data)));
                }
                else static_assert(false,"Not implemented");
            };
            std::visit(add_value,result);
        }
        else continue; //TODO still not accessible getting data without position
    }
    return result;
}

using namespace std::string_literals;

#include <format>
#include <chrono>

osterlib::ContextedError Extract::execute() noexcept{
    ExtractedData result;
    osterlib::ContextedError err;
    if(in_path_.empty()){
        auto matched = Mashroom::instance().data().
            data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().
            match_files(
                last_update_,
                props_.position_.value(),
                props_.center_.value(),
                props_.parameters_,
                props_.from_date_,
                props_.to_date_,
                props_.diff_,
                props_.fcst_unit_,
                props_.level_,
                props_.grid_type_.value()
                );
        
        for(auto& [location,positions]:matched){   
            if(location.type()!=path::TYPE::FILE || !fs::is_regular_file(location.path())){
                err.error(mashroom::errc::not_file);
                err.with_field("procedure","extract").with_field("path",location.path());
                ///@todo log if enabled
                continue;
            }
            if(!fs::exists(location.path())){
                err.error(mashroom::errc::no_exists_path);
                err.with_field("procedure","extract").with_field("file",location.path());
                ///@todo log if enabled
                continue;
            }
            std::cout<<"Extracting from "<<location<<std::endl;
            std::sort(positions.begin(),positions.end(),[](
                const MessagePositionSizeInfo& lhs,
                const MessagePositionSizeInfo& rhs)
            {
                return lhs.begin_<rhs.begin_;
            });
            if(stop_token_.stop_requested()){
                err.error(mashroom::errc::interrupted);
                err.with_field("procedure","extract");
                return err;
            }
            __extract_spec__<
                Data_t::TIME_SERIES,Data_f::GRIB_v1>(
                    fs::path(location.path()),positions,err);
            if(stop_token_.stop_requested()){
                err.error(mashroom::errc::interrupted);
                err.with_field("procedure","extract");
                return err;
            }
        }
    }
    else{
        for(const auto& location:in_path_){
                Mashroom::instance().data().
                    data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().
                    match(
                        location.path(),
                        last_update_,
                        props_.position_.value(),
                        props_.center_.value(),
                        props_.parameters_,
                        props_.from_date_,
                        props_.to_date_,
                        props_.diff_,
                        props_.fcst_unit_,
                        props_.level_,
                        props_.grid_type_.value()
                        );
            __extract__(location.path(),err);
        }
    }
    if(procedures::extract::get_result(result).empty())
        return {};
    return __write_file__(result,output_format_);
}

osterlib::ContextedError Extract::properties_integrity() const noexcept
{
    /*  input path and host are checked in AbstractSearchProcess corresponding methods
        */
    if (out_path_.empty()){
        ContextedError ctx_msg(mashroom::errc::undefined_value);
        ctx_msg.with_field("procedure","extract").with_field("value","out_path");
        return ctx_msg;
    }
    if (!fs::exists(out_path_))
    {
        if (out_path_.has_extension()){
            ContextedError ctx_msg(mashroom::errc::not_directory);
            ctx_msg.with_field("procedure","extract").with_field("value",out_path_.c_str());
            return ctx_msg;
        }
        else if (!fs::create_directories(out_path_)){
            ContextedError ctx_msg(mashroom::errc::create_directory_denied);
            ctx_msg.with_field("procedure","extract").with_field("value",out_path_.c_str());
            return ctx_msg;
        }
    }
    else
    {
        if (!fs::is_directory(out_path_)){
            ContextedError ctx_msg(mashroom::errc::not_directory);
            ctx_msg.with_field("procedure","extract").with_field("value",out_path_.c_str());
            return ctx_msg;
        }
    }
    if (props_.from_date_.has_value() && props_.to_date_.has_value() &&
        !is_correct_interval(*props_.from_date_,*props_.to_date_)){
        ContextedError ctx_msg(mashroom::errc::invalid_argument);
        ctx_msg.with_field("procedure","extract").with_field("arg","date");
        return ctx_msg;
    }
    else if (!props_.position_.has_value()){
        ContextedError ctx_msg(mashroom::errc::undefined_value);
        ctx_msg.with_field("procedure","extract").with_field("value","position");
        return ctx_msg;
    }
    else if (!is_correct_pos(props_.position_.value())){
        ContextedError ctx_msg(mashroom::errc::invalid_argument);
        ctx_msg.with_field("procedure","extract").with_field("arg","coordinate");
        return ctx_msg;
    }
    else if (!props_.grid_type_.has_value()){
        ContextedError ctx_msg(mashroom::errc::undefined_value);
        ctx_msg.with_field("procedure","extract").with_field("value","grid type");
        return ctx_msg;
    }
    else if (props_.parameters_.empty()){
        ContextedError ctx_msg(mashroom::errc::undefined_value);
        ctx_msg.with_field("procedure","extract").with_field("value","search parameters");
        return ctx_msg;
    }
    if (!props_.position_.has_value() || !props_.position_.value().is_correct_pos()) // actually for WGS84
    {
        ContextedError ctx_msg(mashroom::errc::invalid_argument);
        ctx_msg.with_field("procedure","extract").with_field("arg","rect");
        return ctx_msg;
    }
    return {};
}

ExtractedData Extract::__extract_common__(const fs::path &file,
            const std::vector<MessagePositionSizeInfo>& positions,
            osterlib::ContextedError& err){
    ExtractedData result;
    auto auto_unpack = [this,&file,&result,&err,&positions](auto& data){
        using T = std::decay_t<decltype(data)>;
        if constexpr(std::is_same_v<T,std::monostate>)
            return ExtractedData();
        else {
            auto internal_auto_unpack=[this,&file,&result,&positions,&err]<Data_t TYPE,Data_f FORMAT>(ExtractedValues<TYPE,FORMAT>& internal_data){
                auto loc_result = std::move(__extract_spec__<TYPE,FORMAT>(file,positions,err));
                if(err)
                    return ExtractedData();
                else return loc_result;
            };
            return internal_auto_unpack(data);
        }
    };
    return std::visit(auto_unpack,result);
}