#pragma once
#include <string>
#include <filesystem>
#include <thread>
#include "extracted_data.h"

namespace procedures::extract{
namespace fs = std::filesystem;
ExtractedData read_txt_file(const std::stop_token& stop_token,const fs::path&);

ExtractedData read_json_file(const std::stop_token& stop_token,const fs::path&);

ExtractedData read_bin_file(const std::stop_token& stop_token,const fs::path&);
}