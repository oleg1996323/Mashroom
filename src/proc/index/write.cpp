#include "proc/index/write.h"
#include <string>
#include <string_view>
#include <filesystem>
#include <algorithm>
#include <vector>
#include "data/msg.h"
#include "API/grib1/include/paramtablev.h"
#include "proc/common/gen.h"
#include <filesystem>
#include "filesystem.h"

using namespace std::string_literals;
namespace fs = std::filesystem;

const std::unordered_map<TIME_UNIT_ENUM,std::unordered_set<std::string>> token_to_txt_ = {
    {TIME_UNIT_ENUM::HOUR,{"hour"s,"hours"s}},
    {TIME_UNIT_ENUM::DAY,{"day"s,"days"s}},
    {TIME_UNIT_ENUM::MONTH,{"month"s,"months"s}},
    {TIME_UNIT_ENUM::YEAR,{"year"s,"years"s}},
    {TIME_UNIT_ENUM::LATITUDE,{"lat"s,"latitude"s}},
    {TIME_UNIT_ENUM::LONGITUDE,{"lon"s,"longitude"s}},
    {TIME_UNIT_ENUM::LATLON,{"latlon"s}}
};

const std::unordered_map<std::string,TIME_UNIT_ENUM> txt_to_token_ = {
    {"hour"s,TIME_UNIT_ENUM::HOUR},
    {"hours"s,TIME_UNIT_ENUM::HOUR},
    {"day"s,TIME_UNIT_ENUM::DAY},
    {"days"s,TIME_UNIT_ENUM::DAY},
    {"month"s,TIME_UNIT_ENUM::MONTH},
    {"months"s,TIME_UNIT_ENUM::MONTH},
    {"year"s,TIME_UNIT_ENUM::YEAR},
    {"years"s,TIME_UNIT_ENUM::YEAR},
    {"lat"s,TIME_UNIT_ENUM::LATITUDE},
    {"latitude"s,TIME_UNIT_ENUM::LATITUDE},
    {"lon"s,TIME_UNIT_ENUM::LONGITUDE},
    {"longitude"s,TIME_UNIT_ENUM::LONGITUDE},
    {"latlon"s,TIME_UNIT_ENUM::LATLON}
};

const std::unordered_map<TIME_UNIT_ENUM,std::unordered_set<std::string>>& token_to_txt() noexcept{
    return token_to_txt_;
}
const std::unordered_map<std::string,TIME_UNIT_ENUM>& txt_to_token() noexcept{
    return txt_to_token_;
}

std::vector<TIME_UNIT_ENUM> input_to_token_sequence(const std::string& sequence){
    std::vector<TIME_UNIT_ENUM> result;
    std::size_t pos = 0;
    size_t prev_pos = 0;
    while(prev_pos<sequence.size()){
        pos = sequence.find_first_of(fs::separator(),prev_pos);
        if(auto found = txt_to_token_.find(sequence.substr(prev_pos,pos));found==txt_to_token_.end())
            throw std::invalid_argument("invalid sequence input. Prompt: token \""s+sequence.substr(prev_pos,pos)+"\""s);
        else{
            result.push_back(found->second);
            if(pos!=std::string::npos)
                prev_pos = pos+1;
            else return result;
        }
    }
    return result;
}

bool check_format(std::string_view fmt){
	check_format(input_to_token_sequence(std::string(fmt)));
}
bool check_format(const std::vector<TIME_UNIT_ENUM>& tokens){
    std::unordered_map<TIME_UNIT_ENUM,int> counter;
    for(auto token:tokens){
        if(++counter[token]>1)
            return false;
    }
	return true;
}

/**
 * @return Return the names of created files with registered grib data
 */ 
std::vector<std::pair<fs::path, GribMsgDataInfo>> write_txt_file(const fs::path& output_dir,const std::vector<GribMsgDataInfo>& data_){
	std::vector<std::pair<fs::path, GribMsgDataInfo>> result;
	for(const GribMsgDataInfo& msg_info:data_){
		fs::path cur_path(output_dir);
		FILE* dump_file = NULL;
		for(char ch:output_order_){
			switch (ch)
			{
			case 'd':
			case 'D':
				cur_path/=std::format("{:%d}",msg_info.date);		
				break;
			case 'm':
			case 'M':
				cur_path/=std::format("{:%B}",msg_info.date);
				break;
			case 'h':
			case 'H':
				cur_path/=std::format("{:%H}",msg_info.date);
				break;
			case 'y':
			case 'Y':
				cur_path/=std::format("{:%Y}",msg_info.date);
				break;
			case 'g':
			case 'G':
				if(msg_info.grid_data.has_grid())
					cur_path/=grid_to_abbr(msg_info.grid_data.type());
				else
					cur_path/="wogrid";
			default:
				fprintf(stderr,"Error reading format");
				exit(1);
				break;
			}
		}
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