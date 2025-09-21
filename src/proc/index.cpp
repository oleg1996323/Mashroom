#include "proc/index.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "cmd_parse/functions.h"
#include "sys/application.h"
#include "data/info.h"
#include "filesystem.h"
#include "definitions/def.h"
#include "message.h"
#include "program/mashroom.h"
#include "definitions/path_process.h"
#include "network/common/message/message_process.h"
#include <format>
#include "error_data_print.h"
#include "types/time_interval.h"
bool Index::check_format(std::string_view fmt){
	return std::all_of(fmt.begin(),fmt.end(),[&fmt](char ch)
	{
		return std::count(fmt.begin(),fmt.end(),ch)<=1;
	});
}
namespace fs = std::filesystem;
using namespace std::string_literals;

/**
 * @return Return the names of created files with registered grib data
 */ 
std::vector<std::pair<fs::path, GribMsgDataInfo>> Index::__write__(const std::vector<GribMsgDataInfo>& data_){
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


namespace fs = std::filesystem;

/**
 * @brief Execute message indexing of concrete file.
 */
const GribDataInfo& Index::__index_file__(const fs::path& file){
	HGrib1 grib;
	using namespace API::ErrorData;
	if(grib.open_grib(file)!=API::ErrorData::Code<API::GRIB1>::NONE_ERR){
		result.err = API::ErrorDataPrint::print_error<API::GRIB1>(Code<API::GRIB1>::OPEN_ERROR_X1,"",file.string());
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
				}
			}while(grib.next_message());
			write_res= std::move(__write__(grib_msgs));
		}
		for(const auto& [filename,msg]:write_res)
			result.add_info(path::Storage<false>::file(filename),msg);
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

				result.add_info(path::Storage<false>::file(file),info);
			}
		}while(grib.next_message());
		return result;
	}
}

void Index::execute() noexcept{
	for(const path::Storage<false>& path:in_path_){
		switch(path.type_){
			case path::TYPE::DIRECTORY:
				for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(path.path_)){
					if(entry.is_regular_file() && entry.path().has_extension() && 
					(entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
						std::cout<<entry.path()<<std::endl;
						const GribDataInfo& index_data = __index_file__(entry.path());
					}
					else continue;
				}
				break;
			case path::TYPE::FILE:
				__index_file__(path.path_);
				break;
			case path::TYPE::HOST:
				if(host_ref_only){
					if(path.add_.is<path::TYPE::HOST>()){
						std::cout<<"Indexing references from: "<<"host: "<<path.path_<<" port: "<<path.add_.get<path::TYPE::HOST>().port_<<std::endl;
						Mashroom::instance().request<network::Client_MsgT::INDEX_REF>(true,path.path_,path.add_.get<path::TYPE::HOST>().port_,
						TimeInterval{.to_ = utc_tp::clock::now()},utc_tp());
					}
				}
				else {
					if(path.add_.is<path::TYPE::HOST>()){
						std::cout<<"Indexing from: "<<"host: "<<path.path_<<" port: "<<path.add_.get<path::TYPE::HOST>().port_<<std::endl;
						Mashroom::instance().request<network::Client_MsgT::INDEX>(true,path.path_,path.add_.get<path::TYPE::HOST>().port_,
						TimeInterval{.to_ = utc_tp::clock::now()},utc_tp());
					}
				}
				break;
			default:{
				continue;
			}
		}
	}
	Mashroom::instance().add_data(result);
}

ErrorCode Index::add_in_path(const path::Storage<false>& path){
    if(path.path_.empty())
        return ErrorPrint::print_error(ErrorCode::INTERNAL_ERROR,"empty path",AT_ERROR_ACTION::CONTINUE);
    switch(path.type_){
        case path::TYPE::FILE:
            if(!fs::exists(path.path_))
                return ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,path.path_);
            else if(!fs::is_regular_file(path.path_))
                return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_FILE,"",AT_ERROR_ACTION::CONTINUE,path.path_);
            else in_path_.insert(path);
            break;
        case path::TYPE::DIRECTORY:
            if(!fs::exists(path.path_))
                return ErrorPrint::print_error(ErrorCode::FILE_X1_DONT_EXISTS,"",AT_ERROR_ACTION::CONTINUE,path.path_);
            else if(!fs::is_directory(path.path_))
                return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,path.path_);
            else in_path_.insert(path);
            break;
        case path::TYPE::HOST:
            in_path_.insert(path); //will be checked later at process
    }       
    return ErrorCode::NONE;
}
ErrorCode Index::set_dest_dir(std::string_view dest_directory){
    if(fs::path(dest_directory).has_extension())
        return ErrorPrint::print_error(ErrorCode::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,dest_directory);
    if(!fs::exists(dest_directory))
        if(!fs::create_directories(dest_directory))
            return ErrorPrint::print_error(ErrorCode::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,dest_directory);
    dest_directory_=dest_directory;
    return ErrorCode::NONE;
}