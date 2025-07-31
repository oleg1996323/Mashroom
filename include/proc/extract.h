#pragma once
#include <stdio.h>
#include "definitions/def.h"
#include "sections/section_1.h"
#include <unordered_map>
#include <thread>
#include <filesystem>
#include <format>
#include "sys/application.h"
#include "data//common_data_properties.h"
#include "data//info.h"
#include "serialization.h"
#include "network/client.h"
#include "network/server.h"

struct ExtractedValue
{
    std::chrono::system_clock::time_point time_date;
    float value = UNDEFINED;

    ExtractedValue() = default;
    ExtractedValue(utc_tp time, float val) : time_date(time), value(val) {}
    ExtractedValue(const ExtractedValue &other)
    {
        if (this != &other)
        {
            time_date = other.time_date;
            value = other.value;
        }
    }
    ExtractedValue(ExtractedValue &&other) noexcept
    {
        if (this != &other)
        {
            time_date = other.time_date;
            value = other.value;
        }
    }

    bool operator<(const ExtractedValue &other) const
    {
        return time_date < other.time_date;
    }

    bool operator>(const ExtractedValue &other) const
    {
        return time_date > other.time_date;
    }

    ExtractedValue &operator=(const ExtractedValue &extracted_val)
    {
        if (this != &extracted_val)
        {
            time_date = extracted_val.time_date;
            value = extracted_val.value;
        }
        return *this;
    }
    ExtractedValue &operator=(ExtractedValue &&extracted_val) noexcept
    {
        if (this != &extracted_val)
        {
            time_date = extracted_val.time_date;
            value = extracted_val.value;
        }
        return *this;
    }
};

template <>
struct std::less<ExtractedValue>
{
    bool operator()(const ExtractedValue &lhs, const ExtractedValue &rhs) const
    {
        return lhs < rhs;
    }
    bool operator()(const ExtractedValue &lhs, const ExtractedValue &rhs)
    {
        return lhs < rhs;
    }
};

template <>
class std::hash<ExtractedValue>
{
    size_t operator()(const ExtractedValue &data)
    {
        return std::hash<int64_t>{}(static_cast<int64_t>(duration_cast<hours>(data.time_date.time_since_epoch()).count()) << 32 | static_cast<int64_t>(data.value));
    }
};

using ExtractedData = std::unordered_map<CommonDataProperties, std::vector<ExtractedValue>>;
using namespace std::string_literals;
namespace fs = std::filesystem;

#include "proc/interfaces/abstractsearchprocess.h"
#include "proc/interfaces/abstractthreadinterruptor.h"
#include <netdb.h>
#include <program/data.h>



class Extract : public AbstractSearchProcess, public AbstractThreadInterruptor
{
public:

private:
    TimePeriod t_off_ = TimePeriod(years(0),months(1),days(0),hours(0),minutes(0),std::chrono::seconds(0));
    std::string path_format = std::string("{:%Y") + fs::path::preferred_separator + "%m}";
    std::string file_format = std::string("{}_{}_{:%Y_%m}");
    OutputDataFileFormats output_format_ = OutputDataFileFormats::DEFAULT;
    void __extract__(const fs::path &file, ExtractedData &ref_data, const SublimedDataInfo &positions);
    void __extract__(const fs::path& file, ExtractedData& ref_data);
    ErrorCode __create_dir_for_file__(const fs::path &out_f_name);
    ErrorCode __create_file_and_write_header__(std::ofstream &file, const fs::path &out_f_name, const ExtractedData &result);
    template <typename... ARGS>
    fs::path __generate_name__(OutputDataFileFormats format, ARGS &&...args);
    template <typename... ARGS>
    fs::path __generate_directory__(ARGS &&...args); // TODO expand extract division
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
    ErrorCode set_offset_time_interval(const std::optional<TimePeriod>& t_off) noexcept{
        if(!t_off.has_value())
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE,"time offset",AT_ERROR_ACTION::CONTINUE);
        t_off_ = t_off.value();
        return ErrorCode::NONE;
    }
    ErrorCode set_by_request(const ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB> &form)
    {
        std::optional<TimeFrame> t_fcst_;
        if (form.t_fcst_.has_value())
            set_time_fcst(form.t_fcst_.value());
        if (form.center.has_value())
            set_center(form.center.value());
        else
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE, "undefined center", AT_ERROR_ACTION::CONTINUE);
        if (form.from.has_value())
            set_from_date(form.from.value());
        if (form.to.has_value())
            set_to_date(form.to.value());
        if (form.pos.has_value())
            set_position(form.pos.value());
        else
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE, "undefined grid representation", AT_ERROR_ACTION::CONTINUE);
        if (form.rep_t.has_value())
            set_grid_respresentation(form.rep_t.value());
        else
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE, "undefined grid representation", AT_ERROR_ACTION::CONTINUE);
        if (form.time_off_.has_value())
            set_offset_time_interval(form.time_off_.value());
        if (!form.parameters_.empty())
            for (const auto &param : form.parameters_)
                add_parameter(param);
        else
            return ErrorPrint::print_error(ErrorCode::UNDEFINED_VALUE, "empty parameters", AT_ERROR_ACTION::CONTINUE);
    }
    std::string get_dir_file_outp_format() const noexcept
    {
        std::string file_format_tmp;
        std::string dir_format_tmp;
        if (t_off_.hours_ > std::chrono::seconds(0))
        {
            file_format_tmp += "S%_M%_H%_";
            dir_format_tmp += "S%" + fs::path::preferred_separator;
            dir_format_tmp += "M%" + fs::path::preferred_separator;
            dir_format_tmp += "H%" + fs::path::preferred_separator;
        }
        else if (t_off_.hours_ > minutes(0))
        {
            file_format_tmp += "M%_H%_";
            dir_format_tmp += "M%" + fs::path::preferred_separator;
            dir_format_tmp += "H%" + fs::path::preferred_separator;
        }
        else if (t_off_.hours_ > hours(0))
        {
            file_format_tmp += "H%_";
            dir_format_tmp += "H%" + fs::path::preferred_separator;
        }
        if (t_off_.days_ > days(0))
        {
            file_format_tmp += "d%_m%_";
            dir_format_tmp += "d%" + fs::path::preferred_separator;
            dir_format_tmp += "m%" + fs::path::preferred_separator;
        }
        else if (t_off_.months_ > months(0))
        {
            file_format_tmp += "m%_";
            dir_format_tmp += "m%" + fs::path::preferred_separator;
        }
        file_format_tmp += "Y%";
        dir_format_tmp += "Y%";
        std::reverse(file_format_tmp.begin(), file_format_tmp.end());
        std::reverse(dir_format_tmp.begin(), dir_format_tmp.end());
        file_format_tmp = "{}_{}_{:" + file_format_tmp + "}";
        return "{:"s + dir_format_tmp + "}"s;
    }
};

template <typename... ARGS>
fs::path Extract::__generate_name__(OutputDataFileFormats format, ARGS &&...args)
{
    if (static_cast<std::underlying_type_t<OutputDataFileFormats>>(output_format_) &
            static_cast<std::underlying_type_t<OutputDataFileFormats>>(OutputDataFileFormats::TXT_F) ||
        static_cast<std::underlying_type_t<OutputDataFileFormats>>(output_format_) &
            static_cast<std::underlying_type_t<OutputDataFileFormats>>(OutputDataFileFormats::DEFAULT))
    {
        return std::vformat(file_format + ".txt", std::make_format_args(args...));
    }
    return "any.txt";
}
template <typename... ARGS>
fs::path Extract::__generate_directory__(ARGS &&...args)
{
    return out_path_ / std::vformat(path_format, std::make_format_args(args...));
}