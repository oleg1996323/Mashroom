#include "capitalize.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "cmd_parse/functions.h"
#include "sys/application.h"
#include "types/data_binary.h"
#include "aux_code/directories.h"
#include "def.h"
#include "message.h"

#include <format>
bool Capitalize::check_format(std::string_view fmt){
	return std::all_of(fmt.begin(),fmt.end(),[&fmt](char ch)
	{
		return std::count(fmt.begin(),fmt.end(),ch)<=1;
	});
}
namespace fs = std::filesystem;
using namespace std::string_literals;

//must return the names of created files
std::vector<std::pair<fs::path, GribMsgDataInfo>> Capitalize::__write__(const std::vector<GribMsgDataInfo>& data_){
	std::vector<std::pair<fs::path, GribMsgDataInfo>> result;
	for(const GribMsgDataInfo& msg_info:data_){
		fs::path cur_path(dest_directory_);
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
				if(msg_info.grid_data.has_value())
					cur_path/=to_abbr_representation_type(msg_info.grid_data.value().rep_type);
				else
					cur_path/="NON_GRID";
			default:
				fprintf(stderr,"Error reading format");
				exit(1);
				break;
			}
		}
		if(!fs::exists(cur_path) && !fs::create_directories(cur_path))
			throw std::runtime_error("Unable to create path "s+cur_path.c_str());
		const ParmTable* p_t = parameter_table(msg_info.center,msg_info.table_version,msg_info.parameter);
		if(p_t)
			cur_path/=p_t->name+".grib"s;
		else cur_path/="MissParameter"+".grib"s;
		dump_file = fopen(cur_path.c_str(),"a");
		if(!dump_file)
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


namespace fs = std::filesystem;

const GribDataInfo& Capitalize::__capitalize_file__(const fs::path& file){
	HGrib1 grib;
	if(grib.open_grib(file)!=ErrorCodeData::NONE_ERR){
		result.err = ErrorCodeData::OPEN_ERROR;
		return result;
	}
	if(!output_order_.empty()){
		std::vector<std::pair<fs::path, GribMsgDataInfo>> write_res;
		{
			std::vector<GribMsgDataInfo> grib_msgs;
			do{
				auto msg = grib.message();
				if(msg.has_value()){
					GribMsgDataInfo& info = grib_msgs.emplace_back(	std::move(msg.value().get().section_2_.define_grid()),
												std::move(msg.value().get().section_1_.date()),
												grib.current_message_position(),
												grib.current_message_length().value(),
												msg.value().get().section_1_.IndicatorOfParameter(),
												msg.value().get().section_1_.unit_time_range(),
												msg.value().get().section_1_.center(),
												msg.value().get().section_1_.table_version());
					std::cout<<info.date<<std::endl;
					if(info.grid_data.has_value())
						std::cout<<to_abbr_representation_type(info.grid_data.value().rep_type)<<std::endl;
					std::cout<<parameter_table(info.center,info.table_version,info.parameter)->name<<std::endl;
				}
			}while(grib.next_message());
			write_res= std::move(__write__(grib_msgs));
		}
		for(const auto& [filename,msg]:write_res)
			result.add_info(filename,msg);
		return result;
	}
	else{ //only refer (do not overwrite initial files)
		do{
			auto msg = grib.message();
			if(msg.has_value()){
				GribMsgDataInfo info(	std::move(msg.value().get().section_2_.define_grid()),
										std::move(msg.value().get().section_1_.date()),
										grib.current_message_position(),
										grib.current_message_length().value(),
										msg.value().get().section_1_.IndicatorOfParameter(),
										msg.value().get().section_1_.unit_time_range(),
										msg.value().get().section_1_.center(),
										msg.value().get().section_1_.table_version());
				result.add_info(file,info);
			}
		}while(grib.next_message());
		return result;
	}
}

void Capitalize::execute(){
    for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(from_file_)){
        if(entry.is_regular_file() && entry.path().has_extension() && 
        (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            std::cout<<entry.path()<<std::endl;
            const GribDataInfo& cap_data = __capitalize_file__(entry.path());
        }
        else continue;
    }
    auto tmp_res = std::move(release_result());
    tmp_res.sublime();
    BinaryGribInfo data(std::move(tmp_res),std::string(output_order_));
    data.write(fs::path(dest_directory_)/"binfmt.bin");
}