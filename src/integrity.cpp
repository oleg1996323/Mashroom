#include "integrity.h"
#include <ranges>
#include <future>
#include <thread>
#include <vector>
#include <ranges>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <mutex>
#include <ranges>
#include <numeric>
#include <format>
#include <path_process.h>
#include <network/client.h>
#include <program/mashroom.h>
#include "sys/error_print.h"

using namespace std::chrono;

ErrorCode Integrity::execute(){ //TODO: add search from match if in path not defined
    ErrorCode result = ErrorCode::NONE;
    std::vector<fs::directory_entry> entries;
    for(auto& path:in_path_){
        switch(path.type_){
            case path::TYPE::DIRECTORY:{
                for(const fs::directory_entry& entry: fs::directory_iterator(path.path_))
                    entries.push_back(entry);
                {
                    __process_core__(entries); //temporary solution before paralleling

                    //TODO:
                    // if(entries.size()/cpus>1){ //check if HDD or SSD
                    //     std::vector<std::thread> threads(cpus);

                    //     for(unsigned int cpu = 0;cpu<cpus && entries.size()/cpus>1;++cpu){
                    //         auto r = std::ranges::subrange(entries.begin()+cpu*entries.size()/cpus,
                    //                                                                             entries.begin()+(cpu+1)*entries.size()/cpus<entries.end()?
                    //                                                                             entries.begin()+(cpu+1)*entries.size()/cpus:
                    //                                                                             entries.end()
                    //                                                                             );
                    //         std::mutex mute_at_print;
                    //         threads.at(cpu) = std::move(std::thread([this,r,&mute_at_print]() mutable{
                    //                             __process_core__(std::move(r),&mute_at_print);
                    //                         }));
                    //     }
                    //     for(int i = 0;i<cpus;++i){
                    //         threads.at(i).join();
                    //     }
                    //     threads.clear();
                    // }
                    // else
                    //     __process_core__(entries);
                }
            }
            case path::TYPE::FILE:{
                entries.push_back(fs::directory_entry(path.path_));
                __process_core__(entries);
            }
            case path::TYPE::HOST:{
                hProgram->request(path.path_,network::Message<network::Client_MsgT::CAPITALIZE_REF>());
            }
            default:{

            }
        }
    }
    auto sublimed_grib_data = data_.sublime();
    std::ofstream missing_log(out_path_/missed_data,std::ios::out|std::ios::trunc);
    if(!missing_log.is_open()){
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,(out_path_/missed_data).c_str());
        return ErrorCode::CANNOT_OPEN_FILE_X1;
    }
    std::ofstream accessible_data(out_path_/access_data,std::ios::out|std::ios::trunc);
    if(!accessible_data.is_open()){
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::CONTINUE,(out_path_/access_data).c_str());
        return ErrorCode::CANNOT_OPEN_FILE_X1;
    }
    for(const auto& [filename,file_data]:sublimed_grib_data.data()){
        for(const auto& [cmn,sublimed_data_seq]:file_data){
            for(const auto& sublimed_data:sublimed_data_seq){
                utc_tp beg_period = __get_begin_period__(sublimed_data.from,props_.from_date_);
                utc_tp end_period;
                decltype(sublimed_data.discret) discretion =    duration_cast<std::chrono::seconds>(sublimed_data.discret)>utc_diff(0)?
                                                                duration_cast<std::chrono::seconds>(sublimed_data.discret):std::chrono::seconds(1);
                end_period = __get_next_period__(sublimed_data.from)-discretion;
                if(sublimed_data.from<=beg_period){
                    if(sublimed_data.to<end_period)
                        accessible_data<<std::vformat(this->time_result_format,std::make_format_args(sublimed_data.from))<<"-"<<
                        std::vformat(this->time_result_format,std::make_format_args(sublimed_data.from))<<std::endl;
                    else missing_log<<sublimed_data.to<<"-"<<end_period<<std::endl;
                }
                else {
                    missing_log<<std::vformat(this->time_result_format,std::make_format_args(beg_period))<<"-"<<
                        std::vformat(this->time_result_format,std::make_format_args(sublimed_data.from))<<std::endl;
                    if(sublimed_data.to<end_period)
                        missing_log<<std::vformat(this->time_result_format,std::make_format_args(sublimed_data.to))<<"-"<<
                        std::vformat(this->time_result_format,std::make_format_args(end_period))<<std::endl;
                }
                accessible_data<<std::vformat(this->time_result_format,std::make_format_args(sublimed_data.from))<<"-"<<
                std::vformat(this->time_result_format,std::make_format_args(sublimed_data.from))<<std::endl;
            }
        }
    }
    if(missing_log.tellp()>0)
        result = ErrorCode::INTEGRITY_VIOLATED;
    missing_log.close();
    return result;
}

#include "./include/def.h"
#include "grib1/include/message.h"
#include "data/msg.h"
void Integrity::__process_core__(std::ranges::random_access_range auto&& entries, std::mutex* mute_at_print) {
    for (const fs::directory_entry& entry : entries) {
        if (entry.is_regular_file() && entry.path().has_extension() && 
            (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            {
                if (mute_at_print){
                    std::lock_guard<std::mutex> locked(*mute_at_print);
                    std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                }
                else
                    std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
            }

            HGrib1 grib;
            ErrorCodeData err = grib.open_grib(entry.path());
            if(err!=ErrorCodeData::NONE_ERR){
                if(mute_at_print){
                    std::lock_guard<std::mutex> locked(*mute_at_print);
                    file_errors_.push_back(std::make_pair(entry.path(),err));
                }
                else
                    file_errors_.push_back(std::make_pair(entry.path(),err));
                return;
            }
            do{
                const auto& msg = grib.message();
                if(!msg.has_value())
                    throw std::runtime_error("Message undefined");
                GribMsgDataInfo info(	std::move(msg.value().get().section_2_.define_grid()),
                                            std::move(msg.value().get().section_1_.date()),
                                            grib.current_message_position(),
                                            grib.current_message_length().value(),
                                            msg.value().get().section_1_.IndicatorOfParameter(),
                                            msg.value().get().section_1_.unit_time_range(),
                                            msg.value().get().section_1_.center(),
                                            msg.value().get().section_1_.table_version());
                if(props_.center_.has_value() && props_.center_!=info.center)
                    continue;
                if(!props_.parameters_.empty() && !props_.parameters_.contains(SearchParamTableVersion{.param_=info.parameter,.t_ver_=info.table_version}))
                    continue;
                if(props_.fcst_unit_.has_value() && props_.fcst_unit_!=info.t_unit)
                    continue;
                if(props_.grid_type_.has_value() && (!info.grid_data.has_value() || props_.grid_type_.value()!=info.grid_data->rep_type))
                    continue;
                if(props_.position_.has_value()){
                    if(!info.grid_data.has_value())
                        continue;
                    else if(!pos_in_grid(props_.position_.value(),info.grid_data.value()))
                        continue;
                }
                if(props_.to_date_<info.date || props_.from_date_>info.date)
                    continue;
                {
                    if(mute_at_print){
                        std::lock_guard<std::mutex> locked(*mute_at_print);
                        data_.add_info(path::Storage<false>(entry.path().string(),path::TYPE::FILE),std::move(info));
                    }
                    else data_.add_info(path::Storage<false>(entry.path().string(),path::TYPE::FILE),std::move(info));
                }
            }while(grib.next_message());
        }
    }
    return;
}