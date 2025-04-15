#include "check.h"
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
#include "sys/error_print.h"

using namespace std::chrono;

bool Check::execute(){

    bool result = false;
    std::set<int64_t> found;
    system_clock::time_point from = props_.from_date_.has_value()?props_.from_date_.value():system_clock::time_point();
    system_clock::time_point to = props_.to_date_.has_value()?props_.to_date_.value():system_clock::now();
    std::ranges::iota_view<int64_t,int64_t> time_range;
    if(props_.t_sep_.has_value()){
        if(props_.t_sep_.value()==TimeSeparation::HOUR)
            time_range = std::views::iota(  floor<hours>(from).time_since_epoch().count(),
                                            floor<hours>(to).time_since_epoch().count());
        else if(props_.t_sep_.value()==TimeSeparation::DAY)
            time_range = std::views::iota(  floor<days>(from).time_since_epoch().count(),
                                            floor<days>(to).time_since_epoch().count());
        else if(props_.t_sep_.value()==TimeSeparation::MONTH)
            time_range = std::views::iota(  floor<months>(from).time_since_epoch().count(),
                                            floor<months>(to).time_since_epoch().count());
        else if(props_.t_sep_.value()==TimeSeparation::YEAR)
            time_range = std::views::iota(  floor<years>(from).time_since_epoch().count(),
                                            floor<years>(to).time_since_epoch().count());
        else time_range = std::views::iota( floor<days>(from).time_since_epoch().count(),
                                            floor<days>(to).time_since_epoch().count());
    }
    std::ofstream missing_log(dest_directory_/miss_files_filename,std::ios::out);
    if(!missing_log.is_open()){
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dest_directory_/miss_files_filename).c_str());
        exit((uint)ErrorCode::CANNOT_OPEN_FILE_X1);
    }
    auto initial = from;
    std::vector<fs::directory_entry> entries;
    for(const fs::directory_entry& entry: fs::directory_iterator(root_directory_))
        entries.push_back(entry);
    {
        if(entries.size()/cpus>1){
            std::vector<std::thread> threads(cpus);
            std::vector<std::promise<ProcessResult>> threads_results(cpus);

            for(unsigned int cpu = 0;cpu<cpus && entries.size()/cpus>1;++cpu){
                auto r = std::ranges::subrange(entries.begin()+cpu*entries.size()/cpus,
                                                                                    entries.begin()+(cpu+1)*entries.size()/cpus<entries.end()?
                                                                                    entries.begin()+(cpu+1)*entries.size()/cpus:
                                                                                    entries.end()
                                                                                    );
                std::mutex mute_at_print;
                std::promise<ProcessResult>* prom = &threads_results.at(cpu);
                threads.at(cpu) = std::move(std::thread([r,prom,&mute_at_print,this]() mutable{
                                    prom->set_value(process_core(std::move(r),&mute_at_print));
                                }));
            }
            for(int i = 0;i<cpus;++i){
                ProcessResult res = std::move(threads_results.at(i).get_future().get());
                if(!res.err_files.empty()){
                    std::ofstream corr_files(dest_directory_/errorness_files_filename,std::ios::trunc);
                    if(corr_files.is_open())
                        for(ErrorFiles& err_f:res.err_files){
                            corr_files<<err_f.name<<". Error code: "<<err_f.code<<std::endl;
                        }
                    else
                        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dest_directory_/errorness_files_filename).c_str());
                }
                found.merge(res.found);
                threads.at(i).join();
            }
            threads.clear();
            threads_results.clear();
        }
        else{
            ProcessResult res = process_core(entries);
            if(!res.err_files.empty()){
                std::ofstream err_log(dest_directory_/errorness_files_filename,std::ios::trunc);
                if(err_log.is_open())
                    for(ErrorFiles& err_f:res.err_files){
                        err_log<<err_f.name<<". Error code: "<<err_f.code<<std::endl;
                    }
                else
                    ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dest_directory_/errorness_files_filename).c_str());
            }
            found = res.found;
        }
    }
    
    std::vector<long long> miss_data;
    std::ranges::set_difference(time_range,found,std::back_inserter(miss_data), std::ranges::less());
    if(!miss_data.empty()){
        std::set<std::chrono::year_month> by_months;
        std::transform(miss_data.begin(),miss_data.end(),std::inserter(by_months,by_months.begin()),[](const std::chrono::sys_days& date){
            std::chrono::year_month_day tmp(date);
            return std::chrono::year_month(tmp.year(),tmp.month());
            });
        result = true;
        for(auto date:by_months)
            missing_log<<"("<<date.year()<<","<<date.month()<<")"<<std::endl;
    }
    missing_log.close();
    return result;
}

ProcessResult Check::process_core(std::vector<fs::directory_entry> entries, std::mutex* mute_at_print = nullptr) {
    ProcessResult result;
    for (const fs::directory_entry& entry : entries) {
        if (entry.is_regular_file() && entry.path().has_extension() && 
            (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {

            // int p_line;
            // pBar bar;
            {
                if (mute_at_print){
                    std::lock_guard<std::mutex> locked(*mute_at_print);
                    //p_line = progress_line++;
                    //bar.setline(p_line);
                    //std::cout << "\033[" << p_line<< ";0H"; // Переместить курсор на нужную строку
                    //bar.update(0);
                    //bar.print();
                    std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                }
                else{
                    // p_line = progress_line++;
                    // bar.setline(p_line);
                    // std::cout << "\033[" << p_line<< ";0H"; // Переместить курсор на нужную строку
                    // bar.update(0);
                    // bar.print();
                    std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                }
            }

            HGrib1 grib;
            if(!grib.open_grib(entry.path())){
                result.err_files.push_back({entry.path(),ErrorCodeData::OPEN_ERROR});
                return result;
            }
            do{
                GribMsgDataInfo info(	std::move(grib.message().section_2_.define_grid()),
                                            std::move(grib.message().section_1_.date()),
                                            grib.current_message_position(),
                                            grib.current_message_length(),
                                            grib.message().section_1_.IndicatorOfParameter(),
                                            grib.message().section_1_.unit_time_range(),
                                            grib.message().section_1_.center(),
                                            grib.message().section_1_.subcenter());
                if(props_.common_.has_value()){
                    const CommonDataProperties& common = props_.common_.value();
                    if(common.center_!=info.center || 
                        common.subcenter_!=info.subcenter || 
                        common.parameter_!=info.parameter || 
                        (common.fcst_unit_!=TimeFrame::INDIFFERENT && common.fcst_unit_!=info.parameter))
                        continue;
                }
                if(props_.position_.has_value() && !pos_in_grid(props_.position_.value(),info.grid_data))
                    continue;
                if(props_.grid_type_.has_value() && info.grid_data.rep_type!=props_.grid_type_.value())
                    continue;
                if(props_.from_date_.has_value() && props_.from_date_.value()<)
                result.add_info(info);
            }while(grib.next_message());
            
            for(const auto& [cmn,info]:data_.data()){
                for(const auto& info_el:info){
                    if(info_el.err==ErrorCodeData::NONE_ERR){
                        std::chrono::system_clock::time_point date = date_from_epoque(info_el.from);
                        result.found.insert(std::chrono::sys_days(std::chrono::year_month_day(
                            std::chrono::year(date.year),
                            std::chrono::month(date.month),
                            std::chrono::day(date.day))));
                    }
                    else{
                        if (mute_at_print){
                            std::lock_guard<std::mutex> locked(*mute_at_print);
                            std::cout << "Error occured. Code "<<data.code<< ". Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                            result.err_files.emplace_back(ErrorFiles{entry.path(),data.code});
                            break;
                        }
                        else {
                            std::cout << "Error occured. Code "<<data.code<< ". Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                            result.err_files.emplace_back(ErrorFiles{entry.path(),data.code});
                            break;
                        }
                    }

                    {
                        if (mute_at_print){
                            std::lock_guard<std::mutex> locked(*mute_at_print);
                            // bar.update((double)pos/(double)max_pos*100);
                            // bar.print();
                            std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                        }
                        else {
                            // bar.update((double)pos/(double)max_pos*100);
                            // bar.print();
                            std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                        }
                    }
                }
            }
            check_handler.clear_result();
        }
        
    }
    return result;
}