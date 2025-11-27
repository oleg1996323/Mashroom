#include "proc/extract/read.h"
#include "sys/error_exception.h"
#include "sys/error_code.h"
#include <fstream>
#include "data/def.h"
#include "types/coord.h"
#include <expected>
#include "proc/extract/extracted_value.h"

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

        std::expected<TimeSequence,ErrorCode> parse_header_time_info(std::ifstream& file){
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
}

ExtractedData read_txt_file(const std::stop_token& stop_token,fs::path filename){
    Data_f data_format;
    Data_t data_type;
    if(fs::exists(filename) && fs::is_regular_file(filename)){
        std::ifstream file(filename,std::ifstream::in);
        if(file.is_open()){
            VariantExtractedData data;
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
                            TimeSequence time_seq;
                            if(auto time_info = txt::details::ts::parse_header_time_info(file);!time_info.has_value())
                                throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
                            else time_seq = time_info.value();
                            data.emplace<ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1>>();
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
            
            if(!std::getline(file,buffer) || buffer!="\\header")
                throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
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