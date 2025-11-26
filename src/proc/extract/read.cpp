#include "proc/extract/read.h"
#include "sys/error_exception.h"
#include "sys/error_code.h"
#include <fstream>
#include "data/def.h"

namespace procedures::extract{
namespace fs = std::filesystem;
ExtractedData read_txt_file(const std::stop_token& stop_token,fs::path filename){
    Data_f data_format;
    if(fs::exists(filename) && fs::is_regular_file(filename)){
        std::ifstream file(filename,std::ifstream::in);
        if(file.is_open()){
            ExtractedData data;
            std::string buffer;
            if(!std::getline(file,buffer) || buffer!="\\header")
                throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
            while(std::getline(file,buffer)){
                if(auto df = text_to_data_format(buffer); !df.has_value())
                    throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
                else{
                    data_format = df.value();
                    break;
                }
            }
            //check time series, grid etc.
            else 
                if()
            if(!std::getline(file,buffer) || buffer!="\\header")
                throw ErrorException(ErrorCode::FILE_X1_CORRUPTED_OR_INVALID_FORMAT,std::string_view("header not defined"),filename.c_str());
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