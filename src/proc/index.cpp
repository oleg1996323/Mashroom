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
#include "proc/index/write.h"
#include "proc/index/gen.h"
#include "proc/index/indexdatafileformat.h"

namespace fs = std::filesystem;
using namespace std::string_literals;

/**
 * @return Return the names of created files with registered grib data
 */ 
std::unordered_map<fs::path, std::vector<GribMsgDataInfo>> Index::__write__(const std::vector<GribMsgDataInfo>& data_){
	std::unordered_map<fs::path, std::vector<GribMsgDataInfo>> result;
	auto seq_order = input_to_token_sequence(std::string(output_order_));
	for(const GribMsgDataInfo& msg_info:data_){
		fs::path filename;
		if(output_format_!=IndexDataFileFormats::NATIVE)
			filename = generate_filename(output_format_,seq_order);
		//else @todo make define_native_format() function
			//filename = generate_filename(define_native_format(),seq_order);
		switch (output_format_)
		{
			case IndexDataFileFormats::JSON:
				filename = generate_filename(output_format_,seq_order);
				write_json_file(filename,seq_order,data_);
				break;
			
			default:
				//filename = generate_filename(define_native_format(),seq_order);
				break;
		}
		result[filename].push_back(msg_info);
	}
	return result;
}


namespace fs = std::filesystem;

std::vector<GribMsgDataInfo> process_file(HGrib1& grib_file_handler){
	std::vector<GribMsgDataInfo> grib_msgs;
	do{
		auto msg = grib_file_handler.message();
		if(msg.has_value()){
			GribMsgDataInfo& info = grib_msgs.emplace_back(	std::move(msg.value().get().section_2_.define_grid()),
										std::move(msg.value().get().section_1_.date()),
										grib_file_handler.current_message_position(),
										grib_file_handler.current_message_length().value(),
										msg.value().get().section_1_.IndicatorOfParameter(),
										msg.value().get().section_1_.unit_time_range(),
										msg.value().get().section_1_.center(),
										msg.value().get().section_1_.table_version());
		}
	}while(grib_file_handler.next_message());
	return grib_msgs;
}

/**
 * @brief Execute message indexing of concrete file.
 */
std::vector<GribMsgDataInfo> Index::__index_file__(const fs::path& file){
	HGrib1 grib;
	std::vector<GribMsgDataInfo> res;
	using namespace API::ErrorData;
	if(grib.open_grib(file)!=API::ErrorData::Code<API::GRIB1>::NONE_ERR){
		result.err = API::ErrorDataPrint::print_error<API::GRIB1>(Code<API::GRIB1>::OPEN_ERROR_X1,"",file.string());
		return res;
	}
	res = std::move(process_file(grib));
	if(!output_order_.empty())
		__write__(res);
	return res;
}

void Index::execute() noexcept{
	for(const path::Storage<false>& path:in_path_){
		try{
		switch(path.type_){
			case path::TYPE::DIRECTORY:
				for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(path.path_)){
					if(entry.is_regular_file() && entry.path().has_extension() && 
					(entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
						std::cout<<entry.path()<<std::endl;
						
						for(auto&& msg_info :__index_file__(entry.path()))
							result.add_info(path::Storage<false>::file(entry.path()),std::move(msg_info));
					}
					else continue;
				}
				break;
			case path::TYPE::FILE:
				for(auto&& msg_info :__index_file__(path.path_))
					result.add_info(path::Storage<false>::file(path.path_),std::move(msg_info));
				break;
			case path::TYPE::HOST:
				if(path.add_.is<path::TYPE::HOST>()){
					std::cout<<"Indexing references from: "<<"host: "<<path.path_<<" port: "<<path.add_.get<path::TYPE::HOST>().port_<<std::endl;
					auto add_msg = network::make_additional<network::Client_MsgT::INDEX_REF>();
					add_msg.add_indexation_parameters_structure<Data::TYPE::METEO,Data::FORMAT::GRIB>();
					network::Message<network::Client_MsgT::INDEX_REF> msg(std::move(add_msg));
					auto instance = Mashroom::instance().request<network::Client_MsgT::INDEX_REF>(true,path.path_,path.add_.get<path::TYPE::HOST>().port_,std::move(msg));
					if(!instance)
						return;
					decltype(auto) msg_reply = instance->get_result<network::Server_MsgT::DATA_REPLY_INDEX_REF>(-1);
					for(auto& block:msg_reply.additional().blocks_){
						auto add_data = [&path](auto&& block){
							using decay = std::decay_t<decltype(block)>;
							if constexpr(std::is_same_v<decay,std::monostate>)
								return;
							else if constexpr (std::is_same_v<decay,std::variant_alternative_t<1,IndexResult>>){
								SublimedFormatDataInfo<Data_t::METEO,Data_f::GRIB>::sublimed_data_t d;
								auto& host_data = d[path::Storage<false>::host(path.path_,path.add_.get<path::TYPE::HOST>().port_,utc_tp::clock::now())];
								for(auto& [cmn,sublimed]:block.data_){
									std::decay_t<decltype(host_data)>::iterator found = host_data.find(cmn);
									if(found!=host_data.end())
										found->second=sublimed;
									else host_data[std::make_shared<std::decay_t<decltype(host_data)>::key_type::element_type>(cmn)] = sublimed;
								}
								SublimedFormatDataInfo<Data_t::METEO,Data_f::GRIB> msg_data;
								msg_data.add_data(std::move(d));
								Mashroom::instance().data().add_data(std::move(msg_data));
							}
						};
						std::visit(add_data,block);
					}
					if(host_ref_only){
						//@download filepart
					}
				}
				else continue;
				
				break;
			default:{
				continue;
			}
		}
		}
		catch(const std::exception& err){
			std::cout<<err.what()<<std::endl;
		}
	}
	Mashroom::instance().data().add_data(std::move(result.sublime()));
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