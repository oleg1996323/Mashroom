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
#include "definitions/def.h"
#include "grib1/include/message.h"
#include "data/msg.h"

using namespace std::chrono;

std::pair<std::unordered_set<DataStructVariation>,std::vector<std::pair<path::Storage<false>,API::ErrorData::Code<API::GRIB1>::value>>> 
Integrity::__check_file_data_integrity__(const std::vector<fs::directory_entry>& entries,ErrorCode& err, std::mutex* mute_at_print = nullptr) noexcept{
    std::unordered_set<DataStructVariation> index_result;
    std::vector<std::pair<path::Storage<false>,API::ErrorData::Code<API::GRIB1>::value>> errorness_files;
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
            API::ErrorData::Code<API::GRIB1>::value error_f = grib.open_grib(entry.path());
            if(error_f!=API::ErrorData::Code<API::GRIB1>::NONE_ERR){
                if(mute_at_print){
                    std::lock_guard<std::mutex> locked(*mute_at_print);
                    errorness_files.push_back(std::make_pair(path::Storage<false>::file(entry.path().string()),error_f));
                }
                else
                    errorness_files.push_back(std::make_pair(path::Storage<false>::file(entry.path().string()),error_f));
                continue;
            }
            std::vector<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> index_local;
            do{
                const auto& msg = grib.message();
                if(!msg.has_value()){
                    err=ErrorPrint::print_error(ErrorCode::DATA_NOT_FOUND,"Message undefined",AT_ERROR_ACTION::CONTINUE);
                    errorness_files.push_back(std::make_pair(path::Storage<false>::file(entry.path().string()),API::ErrorData::ErrorCode<API::GRIB1>::BAD_FILE_X1));
                    break;
                }
                FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> info(	std::move(msg.value().get().section_2_.define_grid()),
                                            std::move(msg.value().get().section_1_.reference_time()),
                                            grib.current_message_position(),
                                            grib.current_message_length().value(),
                                            msg.value().get().section_1_.parameter_number(),
                                            msg.value().get().section_1_.time_forecast(),
                                            msg.value().get().section_1_.center(),
                                            msg.value().get().section_1_.table_version(),
                                            msg.value().get().section_1_.level_data(),
                                            error_f);
                if(props_.center_.has_value() && props_.center_!=info.center)
                    continue;
                if(!props_.parameters_.empty() && !props_.parameters_.contains(SearchParamTableVersion{.param_=info.parameter,.t_ver_=info.table_version}))
                    continue;
                if(props_.fcst_unit_.has_value() && props_.fcst_unit_!=info.t_unit)
                    continue;
                if(props_.grid_type_.has_value() && (!info.grid_data || 
                        !info.grid_data->has_grid() || 
                        props_.grid_type_.value()!=
                        info.grid_data->type()))
                    continue;
                if(props_.position_.has_value()){
                    if(!info.grid_data ||
                        !info.grid_data->has_grid())
                        continue;
                    else if(!pos_in_grid(props_.position_.value(),*info.grid_data))
                        continue;
                }
                if(props_.to_date_<info.date || props_.from_date_>info.date)
                    continue;
                if(error_f!=decltype(error_f)::NONE_ERR){
                    errorness_files.push_back(std::make_pair(path::Storage<false>::file(entry.path()),error_f));
                    continue;
                }
                {
                    if(mute_at_print){//parallel further
                        std::lock_guard<std::mutex> locked(*mute_at_print);
                        std::error_code error_loc;
                        index_local.emplace_back(info);
                    }
                    else{
                        std::error_code error_loc;
                        index_local.emplace_back(info);
                    }
                }
            }while(grib.next_message());
            if(!errorness_files.empty() && errorness_files.back().first.path_==entry.path())
                continue;
            else{
                if(index_local.empty())
                    continue;
                if(auto found = index_result.find(std::make_pair<Data_f,Data_t>(Data_f::GRIB_v1,Data_t::TIME_SERIES));found!=index_result.end()){
                    std::error_code error_id;
                    const_cast<DataStructVariation&>(*found).add_data(
                        path::Storage<false>::file(
                            entry.path().string(),
                            utc_tp::clock::now()),
                        index_local,error_id);
                }
                else {
                    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1> structure;
                    std::error_code error_id;
                    structure.add_data(path::Storage<false>::file(entry.path().string(),utc_tp::clock::now()),index_local,error_id);
                    index_result.insert(DataStructVariation(std::move(structure)));
                }
            }
        }
    }
    return std::make_pair(std::move(index_result),std::move(errorness_files));
}

void Integrity::__check_metadata_integrity__(const std::unordered_set<DataStructVariation>& data,ErrorCode& err,std::mutex* mute_at_print = nullptr) noexcept{
    for(const auto& d:data){
        auto check_spec_data = [](const auto& spec_data){
            using T = std::decay_t<decltype(data)>;
            if constexpr(std::is_same_v<T,std::monostate>)
                return;
            else{
                auto loc_check_spec_data = []<Data_t TYPE,Data_f FORMAT>(const DataStruct<TYPE,FORMAT>& loc_spec_data){

                };
                return;
            }
        };
    }
}
void Integrity::__correct_indexation__(const std::unordered_set<DataStructVariation>& data,ErrorCode& err) noexcept{
    //if set option then correct
}

ErrorCode Integrity::execute() noexcept{ //TODO: add search from match if in path not defined
    std::vector<fs::directory_entry> entries;
    ErrorCode err;
    for(auto& path:in_path_){
        switch(path.type_){
            case path::TYPE::DIRECTORY:{
                for(const fs::directory_entry& entry: fs::directory_iterator(path.path_))
                    entries.push_back(entry);
                {
                    auto result = __check_file_data_integrity__(entries,err);
                    if(err==ErrorCode::NONE)
                        __check_metadata_integrity__(result.first,err);

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
                    __check_metadata_integrity__(result.first,err);
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
        err = ErrorCode::INTEGRITY_VIOLATED;
    missing_log.close();
    return err;
}