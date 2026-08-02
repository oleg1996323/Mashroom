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
#include "grib1/paramtableversion.h"
#include "proc/index/gen.h"
#include "proc/index/write/json.h"
#include "sys/error.h"

/**
 * @return Return the names of created files with registered grib data
 */ 
osterlib::ContextedError write_json_file(const fs::path& path,
					std::ranges::range auto&& data_){
	std::ofstream file(path,std::ofstream::ate);
	for(const auto& msg_info:data_){
		if(!file.is_open()){
			osterlib::ContextedError error(mashroom::errc::file_permission_denied);
			error.with_field("file",path.string());
			return error;
		}
		file<<to_json(msg_info);
		file.flush();
	}
	file.close();
	return {};
}