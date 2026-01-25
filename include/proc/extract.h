#pragma once
#include <stdio.h>
#include "definitions/def.h"
#include "sections/section_1.h"
#include <unordered_map>
#include <thread>
#include <filesystem>
#include <format>
#include "sys/application.h"
#include "data/common_data_properties.h"
#include "serialization.h"
#include "network/client.h"
#include "network/server.h"
#include "extract/extracted_data.h"

using namespace std::string_literals;
namespace fs = std::filesystem;

#include "proc/interfaces/abstractsearchprocess.h"
#include "proc/interfaces/abstractthreadinterruptor.h"
#include <netdb.h>
#include <program/data.h>
#include "types/time_period.h"


class Extract : public AbstractSearchProcess, public AbstractThreadInterruptor
{
public:

private:
    DateTimeDiff t_off_ = [](){
        std::error_code err;
        return DateTimeDiff(err,std::chrono::months(1));}();
    mutable std::string path_format;
    mutable std::string file_format;
    OutputDataFileFormats output_format_ = OutputDataFileFormats::DEFAULT;
    template<Data_t T,Data_f F>
    ErrorCode __extract__(const fs::path &file, ExtractedData &ref_data, const std::vector<ptrdiff_t>&);

    ErrorCode __extract__(const fs::path& file, ExtractedData& ref_data);
    ErrorCode __write_file__(ExtractedData& result,OutputDataFileFormats FORMAT) const;
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
    virtual ErrorCode execute() noexcept override final;

    virtual ErrorCode properties_integrity() const noexcept override final
    {
        /*  input path and host are checked in AbstractSearchProcess corresponding methods
         */
        if (out_path_.empty())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE, "Output path for extraction mode", AT_ERROR_ACTION::CONTINUE);
        if (!fs::exists(out_path_))
        {
            if (out_path_.has_extension())
                return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY, "", AT_ERROR_ACTION::CONTINUE, out_path_.c_str());
            else if (!fs::create_directories(out_path_))
                return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED, "", AT_ERROR_ACTION::CONTINUE, out_path_.c_str());
        }
        else
        {
            if (!fs::is_directory(out_path_))
                return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY, "", AT_ERROR_ACTION::CONTINUE, out_path_.c_str());
        }
        if (!is_correct_interval(props_.from_date_, props_.to_date_))
            return ErrorPrint::print_error(ErrorCode::INCORRECT_DATE, "", AT_ERROR_ACTION::CONTINUE);
        else if (!props_.position_.has_value())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE, "Not defined", AT_ERROR_ACTION::CONTINUE);
        else if (!is_correct_pos(props_.position_.value()))
            return ErrorPrint::print_error(ErrorCode::INCORRECT_COORD, "", AT_ERROR_ACTION::CONTINUE);
        else if (!props_.grid_type_.has_value())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE, "Grid type", AT_ERROR_ACTION::CONTINUE);
        else if (props_.parameters_.empty())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE, "Parameters", AT_ERROR_ACTION::CONTINUE);
        if (!props_.position_.has_value() || !props_.position_.value().is_correct_pos()) // actually for WGS84
            return ErrorPrint::print_error(ErrorCode::INCORRECT_RECT, "Rectangle zone in extraction is not defined or is defined incorrectly", AT_ERROR_ACTION::CONTINUE);
        return ErrorCode::NONE;
    }
    void set_output_format(OutputDataFileFormats format)
    {
        output_format_ = format;
    }
    OutputDataFileFormats output_format() const
    {
        return output_format_;
    }
    ErrorCode set_offset_time_interval(const std::optional<DateTimeDiff>& t_off) noexcept{
        if(!t_off.has_value())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"time offset",AT_ERROR_ACTION::CONTINUE);
        t_off_ = t_off.value();
        return ErrorCode::NONE;
    }
    ErrorCode set_by_request(const ExtractRequestForm<Data_t::TIME_SERIES, Data_f::GRIB_v1> &form)
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