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
#include <format>
#include "sys/error_print.h"

using namespace std::chrono;

bool Check::execute(){
    bool result = false;
    std::set<int64_t> found;
    std::ranges::iota_view<int64_t,int64_t> time_range;
    if(props_.t_sep_==TimeSeparation::HOUR)
        time_range = std::views::iota(  floor<hours>(props_.from_date_).time_since_epoch().count(),
                                        floor<hours>(props_.to_date_).time_since_epoch().count());
    else if(props_.t_sep_==TimeSeparation::DAY)
        time_range = std::views::iota(  floor<days>(props_.from_date_).time_since_epoch().count(),
                                        floor<days>(props_.to_date_).time_since_epoch().count());
    else if(props_.t_sep_==TimeSeparation::MONTH)
        time_range = std::views::iota(  floor<months>(props_.from_date_).time_since_epoch().count(),
                                        floor<months>(props_.to_date_).time_since_epoch().count());
    else if(props_.t_sep_==TimeSeparation::YEAR)
        time_range = std::views::iota(  floor<years>(props_.from_date_).time_since_epoch().count(),
                                        floor<years>(props_.to_date_).time_since_epoch().count());
    else time_range = std::views::iota( floor<days>(props_.from_date_).time_since_epoch().count(),
                                        floor<days>(props_.to_date_).time_since_epoch().count());
    std::ofstream missing_log(dest_directory_/miss_files_filename,std::ios::out);
    if(!missing_log.is_open()){
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(dest_directory_/miss_files_filename).c_str());
        exit((uint)ErrorCode::CANNOT_OPEN_FILE_X1);
    }
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
                threads.at(cpu) = std::move(std::thread([this,r,prom,&mute_at_print]() mutable{
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
    std::vector<int64_t> miss_data;
    std::ranges::set_difference(time_range,found,std::back_inserter(miss_data), std::ranges::less());
    if(!miss_data.empty()){
        std::set<system_clock::time_point> by_separation;
        std::transform(miss_data.begin(),miss_data.end(),std::inserter(by_separation,by_separation.begin()),[](int64_t date)->system_clock::time_point
        {            
            return system_clock::time_point(seconds(date));
        });
        result = true;
        for(auto date:by_separation){
            switch (props_.t_sep_)
            {
            case TimeSeparation::HOUR:
                missing_log<<std::format("({:%Y/%m/%d %h})",date);
                break;
            case TimeSeparation::DAY:
            missing_log<<std::format("({:%Y/%m/%d})",date);
                break;
            case TimeSeparation::MONTH:
            missing_log<<std::format("({:%Y/%B})",date);
                break;
            case TimeSeparation::YEAR:
                missing_log<<std::format("({:%Y})",date);
                break;
            default:
                break;
            }
        }
    }
    missing_log.close();
    return result;
}

ProcessResult Check::process_core(std::ranges::random_access_range auto&& entries, std::mutex* mute_at_print) {
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
                if(props_.common_.has_value()){
                    const CommonDataProperties& common = props_.common_.value();
                    if(common.center_!=info.center || 
                        common.table_version_!=info.table_version || 
                        common.parameter_!=info.parameter || 
                        (common.fcst_unit_!=TimeFrame::INDIFFERENT && common.fcst_unit_!=info.parameter))
                        continue;
                }
                if(info.grid_data.has_value()){
                    if(props_.position_.has_value() && !pos_in_grid(props_.position_.value(),info.grid_data.value()))
                        continue;
                    if(props_.grid_type_.has_value() && info.grid_data.value().rep_type!=props_.grid_type_.value())
                        continue;
                }
                if(props_.to_date_<info.date || props_.from_date_>info.date)
                    continue;
            }while(grib.next_message());
            
            for(const auto& [cmn,info]:data_.data()){
                for(const auto& info_el:info){
                    if(info_el.err==ErrorCodeData::NONE_ERR){
                        switch (props_.t_sep_)
                        {
                        case TimeSeparation::HOUR:
                            result.found.insert(duration_cast<hours>(info_el.from.time_since_epoch()).count());
                            break;
                        case TimeSeparation::DAY:
                            result.found.insert(duration_cast<days>(info_el.from.time_since_epoch()).count());
                            break;
                        case TimeSeparation::MONTH:
                            result.found.insert(duration_cast<months>(info_el.from.time_since_epoch()).count());
                            break;
                        case TimeSeparation::YEAR:
                            result.found.insert(duration_cast<years>(info_el.from.time_since_epoch()).count());
                            break;
                        default:
                            break;
                        }
                    }
                    else{
                        if (mute_at_print){
                            std::lock_guard<std::mutex> locked(*mute_at_print);
                            std::cout << "Error occured. Code "<<info_el.err<< ". Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                            result.err_files.emplace_back(ErrorFiles{entry.path(),info_el.err});
                            break;
                        }
                        else {
                            std::cout << "Error occured. Code "<<info_el.err<< ". Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                            result.err_files.emplace_back(ErrorFiles{entry.path(),info_el.err});
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
        }
        
    }
    return result;
}