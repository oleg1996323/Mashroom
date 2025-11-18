#pragma once
#include "gen.h"
#include <unordered_map>
#include <vector>
#include <string_view>
#include <string>
#include <unordered_set>
#include "proc/common/fs.h"
#include "data/msg.h"
#include <filesystem>
#include <algorithm>
#include "API/grib1/include/paramtablev.h"
#include "proc/index/gen.h"
#include "proc/index/write/json.h"
#include "sys/error_print.h"

/**
 * @return Return the names of created files with registered grib data
 */ 
bool write_json_file(const fs::path& path,
					std::ranges::range auto&& data_){
	std::ofstream file(path,std::ofstream::ate);
	for(const auto& msg_info:data_){
		if(!file.is_open()){
			ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,path.c_str());
			return false;
		}
		file<<to_json(msg_info);
		file.flush();
	}
	file.close();
	return true;
}