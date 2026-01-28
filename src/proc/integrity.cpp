#include "proc/integrity.h"
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
#include "definitions/path_process.h"
#include "network/client.h"
#include "program/mashroom.h"
#include "sys/error_print.h"

using namespace std::chrono;

ErrorCode Integrity::execute() noexcept{ //TODO: add search from match if in path not defined
    ErrorCode result = ErrorCode::NONE;
    std::vector<fs::directory_entry> entries;
    std::unordered_set<DataStructVariation> file_indexation;
    ErrorCode err;
    for(auto& path:in_path_){
        switch(path.type_){
            case path::TYPE::DIRECTORY:{
                for(const fs::directory_entry& entry: fs::directory_iterator(path.path_))
                    entries.push_back(entry);
                {
                    auto result = __check_file_data_integrity__(entries,err);
                    if(err==ErrorCode::NONE)
                        file_indexation.insert(std::move(result)); //temporary solution before paralleling

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
                    //                             __check_file_data_integrity__(std::move(r),&mute_at_print);
                    //                         }));
                    //     }
                    //     for(int i = 0;i<cpus;++i){
                    //         threads.at(i).join();
                    //     }
                    //     threads.clear();
                    // }
                    // else
                    //     __check_file_data_integrity__(entries);
                }
                continue;
            }
            case path::TYPE::FILE:{
                entries.push_back(fs::directory_entry(path.path_));
                auto result = __check_file_data_integrity__(entries,err);
                if(err==ErrorCode::NONE)
                    file_indexation.insert(std::move(result)); //temporary solution before paralleling
                continue;
            }
            case path::TYPE::HOST:{
                if(path.add_.is<path::TYPE::HOST>())
                    Mashroom::instance().request<network::Client_MsgT::INDEX_REF>(true,path.path_,path.add_.get<path::TYPE::HOST>().port_);
                continue;
            }
            default:{
                return err;
            }
        }
    }
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
    
    if(missing_log.tellp()>0)
        result = ErrorCode::INTEGRITY_VIOLATED;
    missing_log.close();
    return result;
}



void Integrity::__check_metadata_integrity__(const std::unordered_set<DataStructVariation>& data,ErrorCode& err,std::mutex* mute_at_print = nullptr){
    for(const auto& d:data){
        auto check_spec_data = [](const auto& spec_data){
            using T = std::decay_t<decltype(data)>;
            if constexpr(std::is_same_v<T,std::monostate>)

            else{
                auto loc_check_spec_data = []<Data_t TYPE,Data_f FORMAT>(const DataStruct<TYPE,FORMAT>& loc_spec_data){

                };
            }
        };
    }
}
void Integrity::__correct_indexation__(const std::unordered_set<DataStructVariation>& data,ErrorCode& err){

}

#include "definitions/def.h"
#include "grib1/include/message.h"
#include "data/msg.h"
DataStructVariation Integrity::__check_file_data_integrity__(std::ranges::random_access_range auto&& entries,ErrorCode& err, std::mutex* mute_at_print) noexcept{
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
            API::ErrorData::Code<API::GRIB1>::value err = grib.open_grib(entry.path());
            if(err!=API::ErrorData::Code<API::GRIB1>::NONE_ERR){
                if(mute_at_print){
                    std::lock_guard<std::mutex> locked(*mute_at_print);
                    file_errors_.push_back(std::make_pair(entry.path(),err));
                }
                else
                    file_errors_.push_back(std::make_pair(entry.path(),err));
                continue;
            }
            do{
                const auto& msg = grib.message();
                if(!msg.has_value()){
                    err=ErrorPrint::print_error(ErrorCode::DATA_NOT_FOUND,"Message undefined",AT_ERROR_ACTION::CONTINUE);
                    return;
                }
                GribMsgDataInfo info(	std::move(msg.value().get().section_2_.define_grid()),
                                            std::move(msg.value().get().section_1_.reference_time()),
                                            grib.current_message_position(),
                                            grib.current_message_length().value(),
                                            msg.value().get().section_1_.parameter_number(),
                                            msg.value().get().section_1_.time_forecast(),
                                            msg.value().get().section_1_.center(),
                                            msg.value().get().section_1_.table_version(),
                                            msg.value().get().section_1_.level_data());
                if(props_.center_.has_value() && props_.center_!=info.center)
                    continue;
                if(!props_.parameters_.empty() && !props_.parameters_.contains(SearchParamTableVersion{.param_=info.parameter,.t_ver_=info.table_version}))
                    continue;
                if(props_.fcst_unit_.has_value() && props_.fcst_unit_!=info.t_unit)
                    continue;
                if(props_.grid_type_.has_value() && (!info.grid_data.has_grid() || props_.grid_type_.value()!=info.grid_data.type()))
                    continue;
                if(props_.position_.has_value()){
                    if(!info.grid_data.has_grid())
                        continue;
                    else if(!pos_in_grid(props_.position_.value(),info.grid_data))
                        continue;
                }
                if(props_.to_date_<info.date || props_.from_date_>info.date)
                    continue;
                {
                    if(mute_at_print){
                        std::lock_guard<std::mutex> locked(*mute_at_print);
                        data_.add_info(path::Storage<false>::file(entry.path().string()),std::move(info));
                    }
                    else data_.add_info(path::Storage<false>::file(entry.path().string()),std::move(info));
                }
            }while(grib.next_message());
        }
    }
    return ErrorCode::NONE;
}