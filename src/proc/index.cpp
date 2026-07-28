#include "proc/index.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "sys/application.h"
#include "OsterLib/filesystem.h"
#include "definitions/def.h"
#include "grib1/message.h"
#include "program/mashroom.h"
#include "Location.h"
#include <format>
#include "OsterLib/types/time_interval.h"
#include "proc/index/write.h"
#include "proc/index/gen.h"
#include "proc/index/indexdatafileformat.h"
#include "grib1/message.h"

namespace fs = std::filesystem;
using namespace std::string_literals;

mashroom::errc add_data(const DataStructVariation& data){
	mashroom::errc err;
	auto lambda = [&err](const auto& val){
		using T = std::decay_t<decltype(val)>;
		if constexpr(std::is_same_v<std::monostate,T>){
			err=mashroom::errc::INVALID_ARGUMENT;
			return;
		}
		else Mashroom::instance().data().update_indexing(val);
	};
	std::visit(lambda,data);
	return err;
}

/**
 * @return Return the names of created files with registered grib data
 */ 
template<Data_t TYPE,Data_f FORMAT>
std::pair<fs::path,std::vector<data::FileMsg<TYPE,FORMAT>>> Index::__write_file__(const std::vector<data::FileMsg<TYPE,FORMAT>>& data_){
	std::pair<fs::path,std::vector<data::FileMsg<TYPE,FORMAT>>> result;
	if(!fs::exists(dest_directory_.value()))
		throw std::runtime_error("Unavailable write directory"s + dest_directory_->c_str());
	for(const data::FileMsg<TYPE,FORMAT>& msg:data_){
		fs::path filename;
		switch (output_format_)
		{
			case IndexOutputFileFormat::JSON:{
				if constexpr(TYPE==Data_t::TIME_SERIES && FORMAT==Data_f::GRIB_v1){
					if(result.first.empty()){
						auto param = parameter_table(msg.center,msg.table_version,msg.parameter);
						if(param)
							filename = index_gen::generate_filename(output_format_,
								msg.date,center_to_abbr(msg.center),
								grid_to_abbr(msg.grid_data->type()),
								param->name,msg.table_version,
								system_clock::now());
						else filename = index_gen::generate_filename(output_format_,
								msg.date,center_to_abbr(msg.center),
								grid_to_abbr(msg.grid_data->type()),
								msg.table_version,
								system_clock::now());
					}
					if(write_json_file(result.first,data_))
						result.second.push_back(msg);
				}
				else static_assert(false,"Not implemented");
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

std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> process_file(API::HGrib1& grib_file_handler){
	std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> grib_msgs;
	do{
		auto msg = grib_file_handler.message();
		if(msg.has_value()){
			data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>& info = grib_msgs.emplace_back(std::move(msg.value().get().section_2_.define_grid()),
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
template<Data_t TYPE,Data_f FORMAT>
std::vector<data::FileMsg<TYPE,FORMAT>> Index::__index_file__(const fs::path& file){
	API::HGrib1 grib;
	std::vector<data::FileMsg<TYPE,FORMAT>> res;
	using namespace API::ErrorData;
	if(grib.open_grib(file)!=API::ErrorData::Code<API::GRIB1>::NONE_ERR){
		data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> msg;
		msg.err_=API::ErrorDataPrint::print_error<API::GRIB1>(Code<API::GRIB1>::OPEN_ERROR_X1,"",file.string());
		res.emplace_back(std::move(msg));
		return res;
	}
	res = std::move(process_file(grib));
	return res;
}

template<Data_t TYPE,Data_f FORMAT>
std::pair<fs::path,std::vector<data::FileMsg<TYPE,FORMAT>>> Index::__index_write_file__(const fs::path& file){
	API::HGrib1 grib;
	std::pair<fs::path,std::vector<data::FileMsg<TYPE,FORMAT>>> res;
	using namespace API::ErrorData;
	if(grib.open_grib(file)!=API::ErrorData::Code<API::GRIB1>::NONE_ERR){
		data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> msg;
		msg.err_= API::ErrorDataPrint::print_error<API::GRIB1>(Code<API::GRIB1>::OPEN_ERROR_X1,"",file.string());
		res.second.emplace_back(std::move(msg));
		return res;
	}
	return __write_file__(process_file(grib));
}

void Index::execute() noexcept{
	for(const Location<false>& location:in_path_){
		try{
		switch(location.type()){
			case path::TYPE::DIRECTORY:
				if(!fs::is_directory(location.path()))
					return;
				for(std::filesystem::directory_entry entry:std::filesystem::directory_iterator(location.path())){
					if(entry.is_regular_file() && entry.path().has_extension() && 
					(entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
						std::cout<<entry.path()<<std::endl;
						
						if(!dest_directory_.has_value())
							Mashroom::instance().data().
							add_data<Data_t::TIME_SERIES,Data_f::GRIB_v1>(
								Location<false>::file(entry.path(),utc_tp::clock::now()),
								__index_file__<Data_t::TIME_SERIES,Data_f::GRIB_v1>(entry.path())); //@todo able to use different variant types
						else{
							auto write_index_result = __index_write_file__<Data_t::TIME_SERIES,Data_f::GRIB_v1>(entry.path());
							written_.insert(Location<false>::file(write_index_result.first));
							Mashroom::instance().data().
							add_data<Data_t::TIME_SERIES,Data_f::GRIB_v1>(
								Location<false>::file(entry.path()),
								write_index_result.second); //@todo able to use different variant types
						}
					}
					else continue;
				}
				break;
			case path::TYPE::FILE:
				Mashroom::instance().data().add_data<Data_t::TIME_SERIES,Data_f::GRIB_v1>(Location<false>::file(location.path()),
						__index_file__<Data_t::TIME_SERIES,Data_f::GRIB_v1>(location.path())); //@todo able to use different variant types
				break;
			case path::TYPE::HOST:
				if(location.additional().is<path::TYPE::HOST>()){
					std::cout<<"Indexing references from: "<<"host: "<<location.path()<<" port: "<<location.additional().get<path::TYPE::HOST>().port_<<std::endl;
					auto msg = network::Message<network::Client_MsgT::INDEX_REF>();
					msg.add_index<Data_t::TIME_SERIES,Data_f::GRIB_v1>();
					std::error_code err;
					network::ConnectionHandle hconn=Mashroom::instance().connect(err,
						location.path(),
						location.additional().get<path::TYPE::HOST>().port_,
						::app().config().client_config().current_settings());
					auto instance = Mashroom::instance().request(
						hconn,std::monostate(),std::move(msg),std::monostate());
					if(instance->error().has_value()){
						std::cout<<instance->error()->message()<<std::endl;
						return;
					}
					decltype(auto) msg_reply = instance->received()->data_frame();
					if(err!=std::error_code())
						return;
					auto add_data = [&location](auto&& block){
						using decay = std::decay_t<decltype(block)>;
						if constexpr(std::is_same_v<decay,std::monostate>)
							return;
						else if constexpr (std::is_same_v<decay,DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::find_all_t>){
							DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1> d;
							d.add_data(Location<false>::host(location.path(),
								location.additional().get<path::TYPE::HOST>().port_,utc_tp::clock::now()),block);
							Mashroom::instance().data().update_indexing(std::move(d));
						}
					};
					std::visit(add_data,msg_reply.data());
					if(host_ref_only()){
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
}

std::error_code Index::add_in_path(const Location<false>& location){
    if(location.path().empty())
        return std::make_error_code(mashroom::errc::invalid_argument);
    switch(location.type()){
        case path::TYPE::FILE:
            if(!fs::exists(location.path()))
                return std::make_error_code(mashroom::errc::no_exists_path);
            else if(!fs::is_regular_file(location.path()))
                return std::make_error_code(mashroom::errc::not_file);
            else in_path_.insert(location);
            break;
        case path::TYPE::DIRECTORY:
            if(!fs::exists(location.path()))
                return std::make_error_code(mashroom::errc::no_exists_path);
            else if(!fs::is_directory(location.path()))
                return std::make_error_code(mashroom::errc::not_directory);
            else in_path_.insert(location);
            break;
        case path::TYPE::HOST:
            in_path_.insert(location); //will be checked later at process
    }       
    return mashroom::errc::NONE;
}
mashroom::errc Index::set_dest_dir(std::string_view dest_directory){
    if(fs::path(dest_directory).has_extension())
        return ErrorPrint::print_error(mashroom::errc::X1_IS_NOT_DIRECTORY,"",AT_ERROR_ACTION::CONTINUE,dest_directory);
    if(!fs::exists(dest_directory))
        if(!fs::create_directories(dest_directory))
            return ErrorPrint::print_error(mashroom::errc::CREATE_DIR_X1_DENIED,"",AT_ERROR_ACTION::CONTINUE,dest_directory);
    dest_directory_=dest_directory;
    return mashroom::errc::NONE;
}