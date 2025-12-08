#include "proc/extract/read.h"
#include "sys/error_exception.h"
#include "sys/error_code.h"
#include <fstream>
#include "data/def.h"
#include "types/coord.h"
#include <expected>
#include "proc/extract/extracted_value.h"
#include "API/grib1/include/sections/grid/grid.h"
#include "API/grib1/include/paramtablev.h"
#include "boost/algorithm/string.hpp"
#include "serialization.h"
#include "boost_functional/json.h"

namespace procedures::extract{
namespace fs = std::filesystem;

namespace txt::details{
    std::expected<Data_f,ErrorCode> define_format(std::string_view buffer) noexcept{
        if(!buffer.starts_with("format:"))
            return std::unexpected(ErrorCode::NONE);
        if(buffer.size()<std::string_view("format:").size())
            return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
        if(auto df = to_data_format_token(buffer.substr(std::string_view("format:").size())); !df.has_value())
            return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
        else{
            return df.value();
        }
    }

    std::expected<Data_t,ErrorCode> define_type(std::string_view buffer) noexcept{
        if(!buffer.starts_with("type:"))
            return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
        if(buffer.size()<std::string_view("type:").size())
            return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
        if(auto df = to_data_type_token(buffer.substr(std::string_view("type:").size())); !df.has_value())
            return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
        else{
            return df.value();
        }
    }

    namespace ts{
        std::expected<utc_tp,ErrorCode> define_time(std::string_view buffer) noexcept{
            utc_tp time;
            std::istringstream stream(buffer.data());
            stream>>std::chrono::parse("{:%Y/%m/%d %H:%M:%S}",time);
            if(stream.fail())
                return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
            return time;
        }

        std::expected<utc_diff,ErrorCode> define_dtime(std::string_view buffer) noexcept{
            utc_diff diff;
            std::istringstream stream(buffer.data());
            stream>>std::chrono::parse("{:%Y/%m/%d %H:%M:%S}",diff);
            if(stream.fail())
                return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
            return diff;
        }

        std::expected<TimeSequence,ErrorCode> parse_header_time_info(std::istream& file) noexcept{
            std::string buffer;
            utc_tp start;
            utc_tp finish;
            utc_diff diff;
            if(!std::getline(file,buffer) || !buffer.starts_with("start:"))
                return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
            if(auto start_tmp = define_time(buffer.substr(std::string_view("start").size()));
                    !start_tmp.has_value())
                return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
            else finish = start_tmp.value();

            if(!std::getline(file,buffer) || !buffer.starts_with("finish:"))
                return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
            if(auto finish_tmp = define_time(buffer.substr(std::string_view("finish").size()));
                    !finish_tmp.has_value())
                return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
            else finish = finish_tmp.value();

            if(!std::getline(file,buffer) || !buffer.starts_with("dtime:"))
                return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
            if(auto difftmp = define_dtime(buffer.substr(std::string_view("dtime").size()));
                    !difftmp.has_value())
                return std::unexpected(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT);
            else diff = difftmp.value();
            return TimeSequence(start,finish,diff);
        }   
    }

    template<typename T>
    std::expected<T,ErrorCode> define_value_by_token(std::istream& file, std::string& buffer,const char* token) noexcept{
        if(!std::getline(file,buffer) || !buffer.starts_with(token) || buffer.substr(std::string_view(token).size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>){
                auto value_str = std::string_view(buffer).substr(std::strlen(token));
                T result;
                if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),result).ec == std::error_code{})
                    return result;
                else return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            }
            else if constexpr (std::is_enum_v<T>){
                if(auto res = define_value_by_token<std::underlying_type_t<T>>(file,buffer,token);!res.has_value())
                    return std::unexpected(res.error());
                else return static_cast<T>(res.value());
            }
            else static_assert(false,"Not implemented");
        }
    }

    std::expected<int,ErrorCode> define_version(std::istream& file,std::string& buffer) noexcept{
        return define_value_by_token<int>(file,buffer,"version:");
    }

    std::expected<Coord,ErrorCode> define_position(std::istream& file, std::string& buffer) noexcept{
        Coord pos;
        if(auto lat = define_value_by_token<decltype(Coord::lat_)>(file,buffer,"latitude:");!lat.has_value())
            return std::unexpected(lat.error());
        else pos.lat_ = lat.value();
        if(auto lon = define_value_by_token<decltype(Coord::lon_)>(file,buffer,"longitude:");!lon.has_value())
            return std::unexpected(lon.error());
        else pos.lon_ = lon.value();
        return pos;
    }

    std::expected<RepresentationType,ErrorCode> define_grid_type(std::istream& file, std::string& buffer) noexcept{
        return define_value_by_token<RepresentationType>(file,buffer,"grid:");
    }

    std::expected<Organization,ErrorCode> define_center(std::istream& file, std::string& buffer) noexcept{
        return define_value_by_token<Organization>(file,buffer,"center:");
    }

    std::expected<TimeFrame,ErrorCode> define_fcst_unit(std::istream& file, std::string& buffer) noexcept{
        return define_value_by_token<TimeFrame>(file,buffer,"fcst time unit:");
    }

    //table versions and parameters are written as [[*tv*,*param*],...,[*tv*,*param*]]
    std::expected<std::vector<SearchParamTableVersion>,ErrorCode> define_parameters(std::istream& file, std::string& buffer) noexcept{
        std::vector<SearchParamTableVersion> result;
        if(!std::getline(file,buffer) || !buffer.starts_with("parameters:") || buffer.substr(std::string_view("parameters:").size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            auto value_str = std::string_view(buffer).substr(std::string_view("parameters:").size());
            try{
                if(auto json_parse_result = parse_json_from_buffer(value_str);!json_parse_result.has_value())
                    return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                else{
                    const auto& json = json_parse_result.value();
                    if(!json.is_array())
                        return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                    for(auto& item:json.as_array()){
                        if(!item.is_array() && item.as_array().size()!=2)
                            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                        if(!item.as_array().at(0).is_uint64() || !item.as_array().at(1).is_uint64())
                            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                        else {
                            boost::system::error_code err;
                            uint8_t param = item.as_array().at(1).to_number<uint8_t>(err);
                            if(err!=boost::system::error_code{})
                                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                            uint8_t tv = item.as_array().at(0).to_number<uint8_t>(err);
                            if(err!=boost::system::error_code{})
                                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                            result.push_back(SearchParamTableVersion{.param_=param,.t_ver_=tv});
                        }
                    }
                }
            }
            catch(...){
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            }
            return result;
        }
    }


    template<Data_f FORMAT,Data_t TYPE>
    bool valid_version_by_format_type(int version){
        if constexpr (FORMAT == Data_f::GRIB_v1 && TYPE == Data_t::TIME_SERIES){
            return version<=1; //@todo add acceptable versions to file
        }
        else static_assert("Not implemented version check by format and type");
    }

    template<Data_f FORMAT, Data_t TYPE>
    struct HeaderInfo;

    template<>
    struct HeaderInfo<Data_f::GRIB_v1,Data_t::TIME_SERIES>{
        std::vector<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>> commons_;
        TimeSequence time_seq_;
        Coord pos_;
        RepresentationType grid_t_;
    };
    
    template<Data_f FORMAT, Data_t TYPE>
    std::expected<HeaderInfo<FORMAT,TYPE>,ErrorCode> header_by_version_read(std::ifstream& file,std::string& buffer,int version) noexcept{
        if constexpr(TYPE == Data_t::TIME_SERIES){
            HeaderInfo<FORMAT,TYPE> result;
            if(auto time_info = txt::details::ts::parse_header_time_info(file);!time_info.has_value())
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            else result.time_seq_ = time_info.value();
            if constexpr(FORMAT==Data_f::GRIB_v1){
                Organization center;
                if(auto center_res = define_center(file,buffer);!center_res.has_value())
                    return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                else center = center_res.value();
                RepresentationType grid_type;
                if (auto grid_res = define_grid_type(file,buffer);!grid_res.has_value())
                    return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                else result.grid_t_ = grid_res.value();
                Coord pos;
                if(auto pos_res = define_position(file,buffer);!pos_res.has_value())
                    return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                else result.pos_ = std::move(pos_res.value());
                std::vector<SearchParamTableVersion> parameters;
                if(auto parameters_res = define_parameters(file,buffer);!parameters_res.has_value())
                    return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                else{
                    parameters.swap(parameters_res.value());
                    return result;
                }
            }
            else static_assert(false,"Not implemented");
        }
        else static_assert(false,"Not implemented");
    }

    //read the data after header
    template<Data_f FORMAT, Data_t TYPE>
    std::expected<ExtractedValues<TYPE,FORMAT>,ErrorCode> data_by_version_read(std::ifstream& stream,std::string& buffer,const HeaderInfo<FORMAT,TYPE>& header_info,int version) noexcept{
        if constexpr (FORMAT==Data_f::GRIB_v1){
            if constexpr(TYPE==Data_t::TIME_SERIES){
                ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1> result;
                if(version==1){
                    if(!std::getline(stream,buffer))
                        return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                    {
                        std::vector<std::string_view> parameters_names;
                        boost::split(parameters_names,buffer,[](char sep){return sep=='\t' || sep==' ';});
                        //check if all parameters names are correctly defined and ordered
                        if(header_info.commons_.size()!=parameters_names.size())
                            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                        for(const CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& props:header_info.commons_){
                            bool checked = false;
                            if(auto p_ptr = parameter_table(props.center_.value(),props.table_version_.value(),props.parameter_.value());p_ptr==nullptr)
                                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                            else{
                                for(std::string_view p_name:parameters_names)
                                    if(std::string_view(p_ptr->name)==p_name){
                                        checked = true;
                                        break;
                                    }
                            }
                            if(!checked)
                                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                        }
                        for(const CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>& props:header_info.commons_){
                            TimeInterval interval = header_info.time_seq_.get_interval();
                            result[props].reserve((interval.to()-interval.from())/header_info.time_seq_.discret());
                        }
                    }
                    while(std::getline(stream,buffer)){
                        std::vector<std::string_view> string_values;
                        boost::split(string_values,buffer,[](char sep){return sep=='\t' || sep==' ';});
                        try{
                            if(string_values.size()<2)
                                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                            typename ExtractedValue<Data_t::TIME_SERIES,FORMAT>::time_type time;
                            {
                                std::istringstream time_buf(std::string(string_values.front()));
                                time_buf>>std::chrono::parse("{:%Y/%m/%d %H:%M:%S}",time);
                                if(time_buf.fail())
                                    return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                            }
                            for(int i = 1;i<string_values.size();++i){
                                
                                result.at(header_info.commons_.at(i-1)).push_back(ExtractedValue<Data_t::TIME_SERIES,FORMAT>(time,
                                        boost::lexical_cast<typename ExtractedValue<Data_t::TIME_SERIES,FORMAT>::value_t>(string_values.at(i).data(),string_values.at(i).size())));
                            }
                        }
                        catch(const boost::bad_lexical_cast&){
                            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                        }
                    }
                    if(!stream.eof())
                        return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
                    else return result;
                }
                else return std::unexpected(ErrorCode::VERSION_ERROR_X1);
            }
            else static_assert(false,"Not implemented");
        }
        else static_assert(false,"Not implemented");
    }

    template<Data_f FORMAT, Data_t TYPE>
    std::expected<ExtractedData,ErrorCode> format_type_specified_data_read(std::ifstream& file,std::string& buffer) noexcept{
        int version = 0;
        if(auto version_res = define_version(file,buffer);!version_res.has_value())
            return std::unexpected(version_res.error());
        else version = version_res.value();
        if(!valid_version_by_format_type<FORMAT,TYPE>(version))
            return std::unexpected(ErrorCode::VERSION_ERROR_X1);
        if constexpr (FORMAT == Data_f::GRIB_v1 && TYPE == Data_t::TIME_SERIES){
            auto header_result = header_by_version_read<FORMAT,TYPE>(file,buffer,version);
            if(!header_result.has_value())
                return std::unexpected(header_result.error());
            if(!std::getline(file,buffer) || buffer!="\\header")
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            if(auto data_result = data_by_version_read<FORMAT,TYPE>(file,buffer,header_result.value(),version);!data_result.has_value())
                return std::unexpected(data_result.error());
            else return data_result.value();
        }
        else static_assert("Not implemented reading data");
    }
}

ExtractedData read_txt_file(const std::stop_token& stop_token,const fs::path& filename){
    Data_f data_format;
    Data_t data_type;
    if(fs::exists(filename)){
        if(!fs::is_regular_file(filename))
            throw ErrorException(ErrorCode::X1_IS_NOT_FILE,std::string_view(),filename.c_str());
        std::ifstream file(filename,std::ifstream::in);
        if(file.is_open()){
            ExtractedData data;
            std::string buffer;
            if(!std::getline(file,buffer) || buffer!="\\header")
                throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
            while(std::getline(file,buffer)){
                if(auto format = txt::details::define_format(buffer);format.has_value())
                    data_format = format.value();
                else if (format.error()==ErrorCode::NONE)
                    continue;
                else
                    throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
            }
            if(auto type = txt::details::define_type(buffer);type.has_value())
                    data_type = type.value();
            else
                throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
            
            switch (data_type)
            {
            case Data_t::TIME_SERIES:{
                switch (data_format)
                {
                    case Data_f::GRIB_v1:{
                        if(auto data_result = txt::details::format_type_specified_data_read<Data_f::GRIB_v1,Data_t::TIME_SERIES>(file,buffer)){
                            throw ErrorException(ErrorCode::FILE_X1_READING_ERROR,std::string_view("cannot read data"),filename.c_str());
                            return data;
                        }
                        else {
                            data = std::move(data_result.value());
                            return data;
                        }
                        break;
                    }
                    //case ...@todo
                    default:
                        throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
                        break;
                }
                
                break;
            }
            case Data_t::POLYGONE:
                break;
            case Data_t::ISOLINES:
                break;
            case Data_t::GRID:
                break;
            default:
                throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
                break;
            }
            
            //visitor
            return data;
        }
        else{
            throw ErrorException(ErrorCode::CANNOT_OPEN_FILE_X1,std::string_view(""),filename.c_str());
        }
    }
    else{
        throw ErrorException(ErrorCode::FILE_X1_DONT_EXISTS,std::string_view(""),filename.c_str());
    }
}

ExtractedData read_json_file(const std::stop_token& stop_token,const fs::path& filename){
    //static_assert(false,"implementation needed");
}

ExtractedData read_bin_file(const std::stop_token& stop_token,const fs::path& filename){
    using namespace serialization;
    using namespace std::string_view_literals;
    Data_f fmt;
    Data_t type;
    RepresentationType grid_t;
    Coord pos;
    std::ifstream file(filename,std::ifstream::in);
    if(!file.is_open()){
        if(!fs::exists(filename))
            throw ErrorException(ErrorCode::FILE_X1_DONT_EXISTS,""sv,filename.c_str());
        else if(!fs::is_regular_file(filename))
            throw ErrorException(ErrorCode::X1_IS_NOT_FILE,""sv,filename.c_str());
        else throw ErrorException(ErrorCode::CANNOT_OPEN_FILE_X1,""sv,filename.c_str());
    }
    if(auto ser_err = deserialize_from_file(type,file);ser_err!=SerializationEC::NONE)
        throw ErrorException(ErrorCode::FILE_X1_READING_ERROR,""sv,filename.c_str());
    if(auto ser_err = deserialize_from_file(fmt,file);ser_err!=SerializationEC::NONE)
        throw ErrorException(ErrorCode::FILE_X1_READING_ERROR,""sv,filename.c_str());
    if(auto ser_err = deserialize_from_file(grid_t,file);ser_err!=SerializationEC::NONE)
        throw ErrorException(ErrorCode::FILE_X1_READING_ERROR,""sv,filename.c_str());
    if(auto ser_err = deserialize_from_file(pos,file);ser_err!=SerializationEC::NONE)
        throw ErrorException(ErrorCode::FILE_X1_READING_ERROR,""sv,filename.c_str());
    ExtractedData result;
    switch(fmt){
        case Data_f::GRIB_v1:{
            switch(type){
                case Data_t::TIME_SERIES:{
                    ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1> values;
                    if(auto ser_err = deserialize_from_file(values,file);ser_err!=SerializationEC::NONE)
                        throw ErrorException(ErrorCode::FILE_X1_READING_ERROR,""sv,filename.c_str());
                    else result = std::move(values);
                    return result;
                }
                default:{
                    throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,"data type"sv,filename.c_str());
                }
            }
        }
        default:{
            throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,"data format"sv,filename.c_str());
        }
    }
    // deserialize_from_file()
}
}