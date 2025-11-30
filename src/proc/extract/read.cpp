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

    std::expected<int,ErrorCode> define_version(std::string_view buffer) noexcept{
        if(!buffer.starts_with("version:") || buffer.substr(std::string_view("version:").size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            auto value_str = buffer.substr(std::string_view("version:").size());
            int result;
            if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),result).ec == std::error_code{})
                return result;
            else return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        }
    }

    std::expected<Coord,ErrorCode> define_position(std::istream& file, std::string& buffer) noexcept{
        Coord pos;
        if(!std::getline(file,buffer) || !buffer.starts_with("latitude:") || buffer.substr(std::string_view("latitude:").size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            auto value_str = buffer.substr(std::string_view("latitude:").size());
            if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),pos.lat_).ec != std::error_code{})
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        }
        if(!std::getline(file,buffer) || !buffer.starts_with("longitude:") || buffer.substr(std::string_view("longitude:").size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            auto value_str = buffer.substr(std::string_view("longitude:").size());
            if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),pos.lon_).ec != std::error_code{})
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            else return pos;
        }
    }

    std::expected<RepresentationType,ErrorCode> define_grid_type(std::istream& file, std::string& buffer) noexcept{
        RepresentationType grid_t;
        if(!std::getline(file,buffer) || !buffer.starts_with("grid:") || buffer.substr(std::string_view("grid:").size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            auto value_str = buffer.substr(std::string_view("grid:").size());
            if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),reinterpret_cast<std::underlying_type_t<RepresentationType>&>(grid_t)).ec != std::error_code{})
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            else return grid_t;
        }
    }

    std::expected<Organization,ErrorCode> define_center(std::istream& file, std::string& buffer) noexcept{
        Organization center;
        if(!std::getline(file,buffer) || !buffer.starts_with("center:") || buffer.substr(std::string_view("center:").size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            auto value_str = buffer.substr(std::string_view("center:").size());
            if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),reinterpret_cast<std::underlying_type_t<Organization>&>(center)).ec != std::error_code{})
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            else return center;
        }
    }

    std::expected<TimeFrame,ErrorCode> define_fcst_unit(std::istream& file, std::string& buffer) noexcept{
        TimeFrame fcst_unit;
        if(!std::getline(file,buffer) || !buffer.starts_with("fcst time unit:") || buffer.substr(std::string_view("fcst time unit:").size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            auto value_str = buffer.substr(std::string_view("fcst time unit:").size());
            if(std::from_chars(value_str.data(),value_str.data()+value_str.size(),reinterpret_cast<std::underlying_type_t<TimeFrame>&>(fcst_unit)).ec != std::error_code{})
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            else return fcst_unit;
        }
    }

    std::expected<std::vector<SearchParamTableVersion>,ErrorCode> define_parameters(std::istream& file, std::string& buffer) noexcept{
        std::vector<SearchParamTableVersion> result;
        if(!std::getline(file,buffer) || !buffer.starts_with("parameters:") || buffer.substr(std::string_view("parameters:").size()).empty())
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            auto value_str = buffer.substr(std::string_view("parameters:").size());
            try{
                boost::split(result,value_str,[](std::string_view splitted){
                    if(auto pos = splitted.find_first_of(','); pos==std::string_view::npos)
                        throw std::exception();
                    else {
                        uint8_t tv = 0;
                        uint8_t param = 0;
                        if(std::from_chars(splitted.substr(0,pos).data(),splitted.substr(0,pos).data()+pos,tv).ec != std::error_code{})
                            throw std::exception();
                        if(std::from_chars(splitted.substr(pos+1,std::string_view::npos).data(),
                                splitted.substr(pos+1,std::string_view::npos).data()+splitted.substr(pos+1,std::string_view::npos).size(),param).ec != std::error_code{})
                            throw std::exception();
                        else return SearchParamTableVersion{.param_=param,.t_ver_ = tv};
                    }
                });
            }
            catch(...){
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            }
            return result;
        }
    }


    template<Data_f FORMAT,Data_t TYPE>
    bool valid_version_by_format_type(int version){
        if constexpr (FORMAT == Data_f::GRIB_v1 && TYPE == Data_t::TIME_SERIES) noexcept{
            return version<=1; //@todo add acceptable versions to file
        }
        else static_assert("Not implemented version check by format and type");
    }

    template<Data_f FORMAT, Data_t TYPE>
    struct HeaderInfo{
        std::vector<CommonDataProperties<TYPE,FORMAT>> commons_;
        TimeSequence time_seq_;
        Coord pos_;
        RepresentationType grid_t_;
    };
    
    template<Data_f FORMAT, Data_t TYPE>
    std::expected<HeaderInfo<FORMAT,TYPE>,ErrorCode> header_by_version_read(std::ifstream& file,std::string& buffer,int version) noexcept{
        if constexpr(TYPE&Data_t::TIME_SERIES){
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
                else parameters.swap(parameters_res.value());
                CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
            }
        }
        
    }

    template<Data_f FORMAT, Data_t TYPE>
    std::expected<int,ErrorCode> data_by_version_read(std::ifstream& stream,std::string& buffer,int version) noexcept{

    }

    template<Data_f FORMAT, Data_t TYPE>
    std::expected<ExtractedData,ErrorCode> format_type_specified_data_read(std::ifstream& file,std::string& buffer) noexcept{
        int version = 0;
        if(!getline(file,buffer))
            return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
        else {
            if(auto version_res = define_version(buffer);!version_res.has_value())
                return std::unexpected(version_res.error());
            else version = version_res.value();
        }
        if(!valid_version_by_format_type<FORMAT,TYPE>(version))
            return std::unexpected(ErrorCode::VERSION_ERROR_X1);
        ExtractedData data;
        if constexpr (FORMAT == Data_f::GRIB_v1 && TYPE == Data_t::TIME_SERIES){
            if(auto header_result = header_by_version_read<FORMAT,TYPE>(file,buffer,version);!header_result.has_value())
                return std::unexpected(header_result.error());
            if(!std::getline(file,buffer) || buffer!="\\header")
                return std::unexpected(ErrorCode::FILE_X1_READING_ERROR);
            if(auto data_result = data_by_version_read<FORMAT,TYPE>(file,buffer,version);!data_result.has_value())
                return std::unexpected(header_result.error());
            else data.emplace(data_result.value());
        }
        else static_assert("Not implemented reading data");
    }
}

ExtractedData read_txt_file(const std::stop_token& stop_token,fs::path filename){
    Data_f data_format;
    Data_t data_type;
    if(fs::exists(filename) && fs::is_regular_file(filename)){
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
                        if(auto data_result = txt::details::format_type_specified_data_read<Data_f::GRIB_v1,Data_t::TIME_SERIES>(file,buffer))
                            throw ErrorException(ErrorCode::FILE_X1_READING_ERROR,std::string_view("cannot read data"),filename.c_str());
                        else {
                            data = std::move(data_result.value());
                            return data;
                        }
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
    }
}

ExtractedData read_json_file(const std::stop_token& stop_token,fs::path filename){
    static_assert(false,"implementation needed");
}

ExtractedData read_bin_file(const std::stop_token& stop_token,fs::path filename){
    static_assert(false,"implementation needed");
}

}