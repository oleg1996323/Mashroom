#include "proc/index/write.h"
#include <string>
#include <string_view>
#include <filesystem>
#include <algorithm>
#include <vector>
#include "API/grib1/include/paramtablev.h"
#include "proc/index/gen.h"
#include "proc/index/write/json.h"

using namespace std::string_literals;
namespace fs = std::filesystem;


bool check_format(std::string_view fmt){
	return check_format(input_to_token_sequence(std::string(fmt)));
}
bool check_format(const std::vector<DIR_TREE_TOKEN>& tokens){
    std::unordered_map<DIR_TREE_TOKEN,int> counter;
    for(auto token:tokens){
        if(++counter[token]>1)
            return false;
    }
	return true;
}

/**
 * @return Return the names of created files with registered grib data
 */ 
std::vector<std::pair<fs::path, GribMsgDataInfo>> write_json_file(const fs::path& output_dir,
																const std::vector<DIR_TREE_TOKEN>& hierarchy,
																const std::vector<GribMsgDataInfo>& data_){
	std::vector<std::pair<fs::path, GribMsgDataInfo>> result;
	std::ofstream file;
	for(const GribMsgDataInfo& msg_info:data_){
		fs::path cur_path(output_dir);
		FILE* dump_file = NULL;
		make_file(file,generate_filename(IndexDataFileFormats::JSON,hierarchy,msg_info.date));
		if(!fs::exists(cur_path) && !fs::create_directories(cur_path))
			// @todo
			throw std::runtime_error("Unable to create path "s+cur_path.c_str());
		const ParmTable* p_t = parameter_table(msg_info.center,msg_info.table_version,msg_info.parameter);
		if(p_t)
			cur_path/=p_t->name+".grib"s;
		else cur_path/="woparam"+".grib"s;
		dump_file = fopen(cur_path.c_str(),"a");
		if(!dump_file)
			// @todo
			throw std::runtime_error("Unable to open file "s+cur_path.c_str());
		//may be usefull to separate in a unique function for C++ use
		//info can be lost if not added to binary (must be added time/date or coordinate (depend of fmt))
		fwrite(&msg_info.buf_pos_,sizeof(msg_info.buf_pos_),msg_info.msg_sz_,dump_file);
		fclose(dump_file);
		result.push_back({cur_path,msg_info});
		dump_file = NULL;
	}
	return result;
}