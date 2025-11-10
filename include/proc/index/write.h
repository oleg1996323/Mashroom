#pragma once
#include "gen.h"
#include <unordered_map>
#include <vector>
#include <string_view>
#include <string>
#include <unordered_set>
#include "proc/common/fs.h"
#include "data/msg.h"


bool check_format(std::string_view fmt);
bool check_format(const std::vector<DIR_TREE_TOKEN>& tokens);

/**
 * @return Return the names of created files with registered grib data
 */ 
std::vector<std::pair<fs::path, GribMsgDataInfo>> write_json_file(const fs::path& output_dir,
																const std::vector<DIR_TREE_TOKEN>& hierarchy,
																const std::vector<GribMsgDataInfo>& data_);