#include "proc/index.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "cmd_parse/functions.h"
#include "sys/application.h"
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

ErrorCode add_data(const std::unique_ptr<AbstractDataStruct>& data){
	if(!data)
		return ErrorCode::INTERNAL_ERROR;
	if(data->data_type()==Data_t::TIME_SERIES && data->format_type()==Data_f::GRIB_v1)
		Mashroom::instance().data().add_data(*dynamic_cast<DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>*>(data.get()));
}

/**
 * @return Return the names of created files with registered grib data
 */ 
std::pair<fs::path,std::vector<GribMsgDataInfo>> Index::__write_file__(const std::vector<GribMsgDataInfo>& data_){
	std::pair<fs::path,std::vector<GribMsgDataInfo>> result;
	if(!fs::exists(dest_directory_.value()))
		throw std::runtime_error("Unavailable write directory"s + dest_directory_->c_str());
	for(const GribMsgDataInfo& msg_info:data_){
		fs::path filename;
		switch (output_format_)
		{
			case IndexOutputFileFormat::JSON:{
				if(result.first.empty()){
					auto param = parameter_table(msg_info.center,msg_info.table_version,msg_info.parameter);
					if(param)
						filename = index_gen::generate_filename(output_format_,msg_info.date,center_to_abbr(msg_info.center),grid_to_abbr(msg_info.grid_data.type()),param->name,msg_info.table_version,system_clock::now());
					else filename = index_gen::generate_filename(output_format_,msg_info.date,center_to_abbr(msg_info.center),grid_to_abbr(msg_info.grid_data.type()),msg_info.table_version,system_clock::now());
				}
				if(write_json_file(result.first,data_))
					result.second.push_back(msg_info);
			}
				break;
			
			default:
				//filename = generate_filename(define_native_format(),seq_order);
				break;
		}
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
										std::move(msg.value().get().section_1_.reference_time()),
										grib_file_handler.current_message_position(),
										grib_file_handler.current_message_length().value(),
										msg.value().get().section_1_.parameter_number(),
										msg.value().get().section_1_.time_forecast(),
										msg.value().get().section_1_.center(),
										msg.value().get().section_1_.table_version(),
										msg.value().get().section_1_.level_data(),
										msg->get().err_);
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
		res.emplace_back().err_ = API::ErrorDataPrint::print_error<API::GRIB1>(Code<API::GRIB1>::OPEN_ERROR_X1,"",file.string());
		return res;
	}
	res = std::move(process_file(grib));
	return res;
}

std::pair<fs::path,std::vector<GribMsgDataInfo>> Index::__index_write_file__(const fs::path& file){
	HGrib1 grib;
	std::pair<fs::path,std::vector<GribMsgDataInfo>> res;
	using namespace API::ErrorData;
	if(grib.open_grib(file)!=API::ErrorData::Code<API::GRIB1>::NONE_ERR){
		res.second.emplace_back().err_ = API::ErrorDataPrint::print_error<API::GRIB1>(Code<API::GRIB1>::OPEN_ERROR_X1,"",file.string());
		return res;
	}
	return __write_file__(process_file(grib));
}

void Index::execute() noexcept{
	for(const path::Storage<false>& path:in_path_){
		try{
		switch(path.type_){
			case path::TYPE::DIRECTORY:
				if(!fs::is_directory(path.path_))
					return;
				for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(path.path_)){
					if(entry.is_regular_file() && entry.path().has_extension() && 
					(entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
						std::cout<<entry.path()<<std::endl;
						
						if(!dest_directory_.has_value()){
							for(auto&& msg_info :__index_file__(entry.path()))
								result.add_info(path::Storage<false>::file(entry.path()),std::move(msg_info));
						}
						else{
							auto write_index_result = __index_write_file__(entry.path());
							written_.insert(path::Storage<false>::file(write_index_result.first));
							for(auto&& msg_info :write_index_result.second)
								result.add_info(path::Storage<false>::file(entry.path()),std::move(msg_info));
						}
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
					add_msg.add_indexation_parameters_structure<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
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
								SublimedFormatDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>::sublimed_data_t d;
								auto& host_data = d[path::Storage<false>::host(path.path_,path.add_.get<path::TYPE::HOST>().port_,utc_tp::clock::now())];
								for(auto& [cmn,sublimed]:block.data_){
									std::decay_t<decltype(host_data)>::iterator found = host_data.find(cmn);
									if(found!=host_data.end())
										found->second=sublimed;
									else host_data[std::make_shared<std::decay_t<decltype(host_data)>::key_type::element_type>(cmn)] = sublimed;
								}
								SublimedFormatDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1> msg_data;
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
	Mashroom::instance().data().add_data(std::move(*result.release()));
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