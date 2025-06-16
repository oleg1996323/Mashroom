#pragma once
#include <stdio.h>
#include "include/def.h"
#include "sections/section_1.h"
#include <unordered_map>
#include <thread>
#include <filesystem>
#include <format>
#include "sys/application.h"
#include "data/common.h"
#include "data/info.h"
#include <functional/serialization.h>

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

#include <abstractsearchprocess.h>
#include <abstracttimeseparation.h>
#include <abstractthreadinterruptor.h>
#include <netdb.h>
#include <program/data.h>

template <Data::TYPE T, Data::FORMAT F>
struct ExtractRequestForm;

template <>
struct ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>
{
    constexpr static Data::TYPE type = Data::TYPE::METEO;
    constexpr static Data::FORMAT format = Data::FORMAT::GRIB;
    std::optional<TimeFrame> t_fcst_;
    std::optional<Organization> center;
    std::optional<utc_tp> from; // requested from Date
    std::optional<utc_tp> to;   // requested to Date
    std::optional<Coord> pos;   // requested position (in WGS)
    std::optional<RepresentationType> rep_t;
    std::optional<TimeOffset> time_off_;
    size_t parameter_size = 0;
    std::vector<SearchParamTableVersion> parameters_;
};

namespace serialization
{
    template <>
    constexpr size_t min_serial_size(const ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB> &val) noexcept
    {
        return min_serial_size(val.type) + min_serial_size(val.type) + min_serial_size(val.center) + min_serial_size(val.from) + min_serial_size(val.to) + min_serial_size(val.pos) +
               min_serial_size(val.rep_t) + min_serial_size(val.time_off_) + min_serial_size(val.parameter_size);
    }

    size_t serial_size(const ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB> &val) noexcept
    {
        return serial_size(val.type) + serial_size(val.type) + serial_size(val.center) + serial_size(val.from) +
               serial_size(val.to) + serial_size(val.pos) + serial_size(val.rep_t) + serial_size(val.time_off_) +
               serial_size(val.parameter_size) + serial_size<decltype(val.parameters_)::value_type()>() * val.parameter_size;
    }

    template <bool NETWORK_ORDER = true>
    bool serialize(const ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>&form, std::vector<char> &buf) noexcept
    {
        if (form.parameters_.size() != form.parameter_size)
            return false;
        serialize(buf,form.type,form.format,form.t_fcst_,form.center,form.from,form.to,form.pos,form.time_off_,form.parameter_size);
        for (const auto &param : form.parameters_)
            if(!serialize<NETWORK_ORDER>(param, buf))
                return false;
        return true;
    }

    template <bool NETWORK_ORDER = true>
    auto deserialize<ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>>(std::span<const char> buf)
        -> std::expected<ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>, SerializationEC>
    {   
        using type = ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>;
        using cont_type = decltype(type::parameters_)::value_type>;
        if (buf.size() < min_serial_size<type>())
            return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
        auto result = deserialize<NETWORK_ORDER,ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>>(
            type::t_fcst_,type::center,type::from,type::to,type::pos,type::time_off_,
            type::parameter_size,
            buf.subspan(serial_size(result.type)+serial_size(result.format)));
        if(!result)
            return std::unexpected(result.error());
        else{
            if(buf.size()<serial_size())
                return std::unexpected(SerializationEC::BUFFER_SIZE_LESSER);
            buf.subspan(min_serial_size<type>());
            result.parameters_.reserve(result.parameter_size);
            for(int i = 0, i<result.parameter_size;++i){
                auto res_cont = deserialize<cont_type,NETWORK_ORDER>(buf);
                if(!res_cont)
                    return std::unexpected(res_cont.error())
                else result.parameters_.push_back(res_cont.value());
                buf = buf.subspan(serial_size<cont_type>());
            }
            return result;
        }
    }

    static_assert(std::is_same_v<decltype(deserialize_network<ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>>({})),
                                 std::expected<ExtractRequestForm<Data::TYPE::METEO, Data::FORMAT::GRIB>, SerializationEC>>,
                  "Return type validation failed");
}

class Extract : public AbstractSearchProcess, public AbstractTimeSeparation, public AbstractThreadInterruptor
{
public:
    enum class ExtractFormat : uint
    {
        DEFAULT = 0,
        TXT_F = 1,
        BIN_F = (1 << 1),
        GRIB_F = (1 << 2),
        ARCHIVED = (1 << 3)
    };

private:
    std::string directory_format = std::string("{:%Y") + fs::path::preferred_separator + "%m}";
    std::string file_format = std::string("{}_{}_{:%Y_%m}");
    ExtractFormat output_format_ = ExtractFormat::DEFAULT;
    void __extract__(const fs::path &file, ExtractedData &ref_data, const SublimedDataInfo &positions);
    ErrorCode __create_dir_for_file__(const fs::path &out_f_name);
    ErrorCode __create_file_and_write_header__(std::ofstream &file, const fs::path &out_f_name, const ExtractedData &result);
    template <typename... ARGS>
    fs::path __generate_name__(Extract::ExtractFormat format, ARGS &&...args);
    template <typename... ARGS>
    fs::path __generate_directory__(ARGS &&...args); // TODO expand extract division
public:
    virtual ErrorCode execute() override final;

    virtual ErrorCode properties_integrity() const override final
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
    void set_output_format(ExtractFormat format)
    {
        output_format_ = format;
    }
    ExtractFormat output_format() const
    {
        return output_format_;
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
    virtual void __set_offset_time_interval__() override final
    {
        std::string file_format_tmp;
        std::string dir_format_tmp;
        if (diff_time_interval_.hours_ > std::chrono::seconds(0))
        {
            file_format_tmp += "S%_M%_H%_";
            dir_format_tmp += "S%" + fs::path::preferred_separator;
            dir_format_tmp += "M%" + fs::path::preferred_separator;
            dir_format_tmp += "H%" + fs::path::preferred_separator;
        }
        else if (diff_time_interval_.hours_ > minutes(0))
        {
            file_format_tmp += "M%_H%_";
            dir_format_tmp += "M%" + fs::path::preferred_separator;
            dir_format_tmp += "H%" + fs::path::preferred_separator;
        }
        else if (diff_time_interval_.hours_ > hours(0))
        {
            file_format_tmp += "H%_";
            dir_format_tmp += "H%" + fs::path::preferred_separator;
        }
        if (diff_time_interval_.days_ > days(0))
        {
            file_format_tmp += "d%_m%_";
            dir_format_tmp += "d%" + fs::path::preferred_separator;
            dir_format_tmp += "m%" + fs::path::preferred_separator;
        }
        else if (diff_time_interval_.months_ > months(0))
        {
            file_format_tmp += "m%_";
            dir_format_tmp += "m%" + fs::path::preferred_separator;
        }
        file_format_tmp += "Y%";
        dir_format_tmp += "Y%";
        std::reverse(file_format_tmp.begin(), file_format_tmp.end());
        std::reverse(dir_format_tmp.begin(), dir_format_tmp.end());
        file_format_tmp = "{}_{}_{:" + file_format_tmp + "}";
        directory_format = "{:" + dir_format_tmp + "}";
    }
};

Extract::ExtractFormat operator|(Extract::ExtractFormat lhs, Extract::ExtractFormat rhs);
Extract::ExtractFormat operator&(Extract::ExtractFormat lhs, Extract::ExtractFormat rhs);
Extract::ExtractFormat &operator|=(Extract::ExtractFormat &lhs, Extract::ExtractFormat rhs);
Extract::ExtractFormat &operator&=(Extract::ExtractFormat &lhs, Extract::ExtractFormat rhs);
Extract::ExtractFormat operator^(Extract::ExtractFormat lhs, Extract::ExtractFormat rhs);
Extract::ExtractFormat operator~(Extract::ExtractFormat val);

template <typename... ARGS>
fs::path Extract::__generate_name__(Extract::ExtractFormat format, ARGS &&...args)
{
    if (static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_) &
            static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::TXT_F) ||
        static_cast<std::underlying_type_t<Extract::ExtractFormat>>(output_format_) &
            static_cast<std::underlying_type_t<Extract::ExtractFormat>>(Extract::ExtractFormat::DEFAULT))
    {
        return std::vformat(file_format + ".txt", std::make_format_args(args...));
    }
    return "any.txt";
}
template <typename... ARGS>
fs::path Extract::__generate_directory__(ARGS &&...args)
{
    return out_path_ / std::vformat(directory_format, std::make_format_args(args...));
}