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
#include "Location.h"
#include "web/client.h"
#include "program/mashroom.h"
#include "definitions/def.h"
#include "grib1/message.h"
#include "data/msg.h"

using namespace std::chrono;

std::pair<std::unordered_set<DataStructVariation>,std::vector<std::pair<Location<false>,osterlib::ContextedError>>> 
Integrity::__check_file_data_integrity__(const std::vector<fs::directory_entry>& entries,std::mutex* mute_at_print = nullptr) noexcept{
    std::unordered_set<DataStructVariation> index_result;
    std::vector<std::pair<Location<false>,osterlib::ContextedError>> errorness_files;
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

            api::HGrib1 grib;
            osterlib::ContextedError error_f = grib.open_grib(entry.path());
            if(error_f){
                if(mute_at_print){
                    std::lock_guard<std::mutex> locked(*mute_at_print);
                    errorness_files.push_back(std::make_pair(Location<false>::file(entry.path().string()),std::move(error_f)));
                }
                else
                    errorness_files.push_back(std::make_pair(Location<false>::file(entry.path().string()),std::move(error_f)));
                continue;
            }
            std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>> index_local;
            do{
                const auto& msg = grib.message();
                if(!msg.has_value()){
                    error_f.error(api::errc<API_T::GRIB1>::bad_file,"message undefined");
                    error_f.with_field("procedure","integrity")
                    .with_field("file",entry.path().c_str());
                    errorness_files.push_back(std::make_pair(Location<false>::file(entry.path().string()),std::move(error_f)));
                    break;
                }
                data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1> info(	std::move(msg.value().get().section_2_.define_grid()),
                                            std::move(msg.value().get().section_1_.reference_time()),
                                            grib.current_message_position(),
                                            grib.current_message_length().value(),
                                            msg.value().get().section_1_.parameter_number(),
                                            msg.value().get().section_1_.time_forecast(),
                                            msg.value().get().section_1_.center(),
                                            msg.value().get().section_1_.table_version(),
                                            msg.value().get().section_1_.level_data(),
                                            error_f.code());
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
                if(error_f){
                    errorness_files.push_back(std::make_pair(Location<false>::file(entry.path()),std::move(error_f)));
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
            if(!errorness_files.empty() && errorness_files.back().first.path()==entry.path())
                continue;
            else{
                if(index_local.empty())
                    continue;
                osterlib::ContextedError ctx_err;
                if(auto found = index_result.find(std::make_pair<Data_f,Data_t>(Data_f::GRIB_v1,Data_t::TIME_SERIES));found!=index_result.end()){
                    const_cast<DataStructVariation&>(*found).add_data(
                        Location<false>::file(
                            entry.path().string(),
                            utc_tp::clock::now()),
                        index_local,ctx_err);
                }
                else {
                    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1> structure;
                    structure.add_data(Location<false>::file(entry.path().string(),utc_tp::clock::now()),index_local,ctx_err);
                    index_result.insert(DataStructVariation(std::move(structure)));
                }
            }
        }
    }
    return std::make_pair(std::move(index_result),std::move(errorness_files));
}

osterlib::ContextedError Integrity::__check_metadata_integrity__(const std::unordered_set<DataStructVariation>& data,std::mutex* mute_at_print = nullptr) noexcept{
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
osterlib::ContextedError Integrity::__correct_indexation__(const std::unordered_set<DataStructVariation>& data) noexcept{
    //if set option then correct
}

osterlib::ContextedError Integrity::execute() noexcept{ //TODO: add search from match if in path not defined
    std::vector<fs::directory_entry> entries;
    osterlib::ContextedError ctx_err;
    for(auto& location:in_path_){
        switch(location.type()){
            case path::TYPE::DIRECTORY:{
                for(const fs::directory_entry& entry: fs::directory_iterator(location.path()))
                    entries.push_back(entry);
                {
                    auto result = __check_file_data_integrity__(entries);
                    ctx_err = __check_metadata_integrity__(result.first);

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
                entries.push_back(fs::directory_entry(location.path()));
                auto result = __check_file_data_integrity__(entries);
                ctx_err = __check_metadata_integrity__(result.first);
                continue;
            }
            default:
            {
                std::cout<<"Unvailable using of host-paths"<<std::endl;
                return ctx_err;
            }
        }
    }
    std::ofstream missing_log(out_path_/missed_data,std::ios::out|std::ios::trunc);
    if(!missing_log.is_open()){
        ctx_err.error(mashroom::errc::file_permission_denied);
        ctx_err
        .with_field("procedure","integrity")
        .with_field("file",(out_path_/missed_data).c_str());
        return ctx_err;
    }
    std::ofstream accessible_data(out_path_/access_data,std::ios::out|std::ios::trunc);
    if(!accessible_data.is_open()){
        ctx_err.error(mashroom::errc::file_permission_denied);
        ctx_err
        .with_field("procedure","integrity")
        .with_field("file",(out_path_/access_data).c_str());
        return ctx_err;
    }
    if(missing_log.tellp()>0){
        ctx_err.error(mashroom::errc::integrity_violated);
        ctx_err
        .with_field("procedure","integrity");
        return ctx_err;
    }
    missing_log.close();
    return ctx_err;
}