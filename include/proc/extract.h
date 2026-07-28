#pragma once
#include <stdio.h>
#include "definitions/def.h"
#include "grib1/sections.h"
#include <unordered_map>
#include <thread>
#include <filesystem>
#include <format>
#include "sys/application.h"
#include "data/common_data_properties.h"
#include "OsterLib/serialization.h"
#include "extract/extracted_data.h"

using namespace std::string_literals;
namespace fs = std::filesystem;

#include "proc/interfaces/abstractsearchprocess.h"
#include "proc/interfaces/abstractthreadinterruptor.h"
#include <netdb.h>
#include "program/data.h"
#include "OsterLib/types/time_period.h"
#include "web/client/message/application/extract.h"
#include "common/MessagePositionSizeInfo.h"

class Extract : public AbstractSearchProcess, public AbstractThreadInterruptor
{
public:

private:
    utc_tp last_update_;
    DateTimeDiff t_off_ = [](){
        std::error_code err;
        return DateTimeDiff(err,std::chrono::months(1));}();
    mutable std::string path_format;
    mutable std::string file_format;
    OutputDataFileFormats output_format_ = OutputDataFileFormats::DEFAULT;
    template<Data_t T,Data_f F>
    ExtractedData __extract_spec__(const fs::path &file,
            const std::vector<MessagePositionSizeInfo>&,mashroom::errc&);
    ExtractedData __extract_common__(const fs::path &file,
            const std::vector<MessagePositionSizeInfo>&,mashroom::errc&);
    ExtractedData __extract__(const fs::path& file, mashroom::errc&);
    mashroom::errc __write_file__(ExtractedData& result,OutputDataFileFormats FORMAT) const;
public:
    Extract() = default;
    Extract(const Extract& other)=delete;
    Extract(Extract&& other) noexcept:
    AbstractSearchProcess(other),
    t_off_(std::move(other.t_off_)),
    path_format(std::move(other.path_format)),
    file_format(std::move(other.file_format)),
    output_format_(std::move(output_format_)){}
    Extract& operator=(const Extract& other)=delete;
    Extract& operator=(Extract&& other) noexcept{
        if(this!=&other){
            AbstractSearchProcess::operator=(other);
            t_off_ = std::move(other.t_off_);
            path_format = std::move(other.path_format);
            file_format = std::move(other.file_format);
            output_format_ = std::move(output_format_);
        }
        return *this;
    }
    virtual mashroom::errc execute() noexcept override final;

    virtual mashroom::errc properties_integrity() const noexcept override final
    {
        /*  input path and host are checked in AbstractSearchProcess corresponding methods
         */
        if (out_path_.empty())
            return ErrorPrint::print_error(mashroom::errc::UNDEFINED_VALUE, "Output path for extraction mode", AT_ERROR_ACTION::CONTINUE);
        if (!fs::exists(out_path_))
        {
            if (out_path_.has_extension())
                return ErrorPrint::print_error(mashroom::errc::X1_IS_NOT_DIRECTORY, "", AT_ERROR_ACTION::CONTINUE, out_path_.c_str());
            else if (!fs::create_directories(out_path_))
                return ErrorPrint::print_error(mashroom::errc::CREATE_DIR_X1_DENIED, "", AT_ERROR_ACTION::CONTINUE, out_path_.c_str());
        }
        else
        {
            if (!fs::is_directory(out_path_))
                return ErrorPrint::print_error(mashroom::errc::X1_IS_NOT_DIRECTORY, "", AT_ERROR_ACTION::CONTINUE, out_path_.c_str());
        }
        if (props_.from_date_.has_value() && props_.to_date_.has_value() &&
            !is_correct_interval(*props_.from_date_,*props_.to_date_))
            return ErrorPrint::print_error(mashroom::errc::INCORRECT_DATE, "", AT_ERROR_ACTION::CONTINUE);
        else if (!props_.position_.has_value())
            return ErrorPrint::print_error(mashroom::errc::UNDEFINED_VALUE, "Not defined", AT_ERROR_ACTION::CONTINUE);
        else if (!is_correct_pos(props_.position_.value()))
            return ErrorPrint::print_error(mashroom::errc::INCORRECT_COORD, "", AT_ERROR_ACTION::CONTINUE);
        else if (!props_.grid_type_.has_value())
            return ErrorPrint::print_error(mashroom::errc::UNDEFINED_VALUE, "Grid type", AT_ERROR_ACTION::CONTINUE);
        else if (props_.parameters_.empty())
            return ErrorPrint::print_error(mashroom::errc::UNDEFINED_VALUE, "Parameters", AT_ERROR_ACTION::CONTINUE);
        if (!props_.position_.has_value() || !props_.position_.value().is_correct_pos()) // actually for WGS84
            return ErrorPrint::print_error(mashroom::errc::INCORRECT_RECT, "Rectangle zone in extraction is not defined or is defined incorrectly", AT_ERROR_ACTION::CONTINUE);
        return mashroom::errc::NONE;
    }
    void set_output_format(OutputDataFileFormats format)
    {
        output_format_ = format;
    }
    void set_last_update(utc_tp last_upd) noexcept{
        last_update_=last_upd;
    }
    utc_tp last_update() const noexcept{
        return last_update_;
    }
    OutputDataFileFormats output_format() const
    {
        return output_format_;
    }
    mashroom::errc set_offset_time_interval(const std::optional<DateTimeDiff>& t_off) noexcept{
        if(!t_off.has_value())
            return ErrorPrint::print_error(mashroom::errc::UNDEFINED_VALUE,"time offset",AT_ERROR_ACTION::CONTINUE);
        t_off_ = t_off.value();
        return mashroom::errc::NONE;
    }
    mashroom::errc set_by_request(const ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1> &form)
    {
        props_ = form.search_props_;
        if(form.file_fmt_.has_value())
            set_output_format(*form.file_fmt_|OutputDataFileFormats::ARCHIVED);
        else set_output_format(OutputDataFileFormats::DEFAULT|OutputDataFileFormats::ARCHIVED);
        if(form.t_separation_.has_value())
            return set_offset_time_interval(*form.t_separation_);
        else{
            std::error_code err;
            return set_offset_time_interval(DateTimeDiff(err,std::chrono::months(1)));
        }
    }
};