#include "proc/extract/read.h"
#include "sys/error_exception.h"
#include "sys/error_code.h"

namespace procedures::extract{
namespace fs = std::filesystem;
ExtractedData read_txt_file(const std::stop_token& stop_token,fs::path){
    static_assert(false,"implementation needed");
}

ExtractedData read_json_file(const std::stop_token& stop_token,fs::path){
    static_assert(false,"implementation needed");
}

ExtractedData read_bin_file(const std::stop_token& stop_token,fs::path){
    static_assert(false,"implementation needed");
}

}