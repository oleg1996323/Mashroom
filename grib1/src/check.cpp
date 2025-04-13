#include "grib1/include/check.h"
#include <ranges>
#include <future>
#include <thread>
#include <vector>
#include <ranges>
#include <algorithm>

const GribDataInfo& Check::execute(){
    // HGrib1 grib;
	// if(!grib.open_grib(from_file_)){
	// 	result.err = ErrorCodeData::OPEN_ERROR;
	// 	return result;
	// }
    // do{
	// 	GribMsgDataInfo info(	std::move(grib.message().section_2_.define_grid()),
	// 								std::move(grib.message().section_1_.date()),
	// 								grib.current_message_position(),
	// 								grib.current_message_length(),
	// 								grib.message().section_1_.IndicatorOfParameter(),
	// 								grib.message().section_1_.unit_time_range(),
	// 								grib.message().section_1_.center(),
	// 								grib.message().section_1_.subcenter());
	// 	result.add_info(info);
	// }while(grib.next_message());
	// return result;

    bool result = false;
    std::set<std::chrono::sys_days> found;
    auto begin_days = std::chrono::sys_days(from);
    auto end_days = std::chrono::sys_days(to)+std::chrono::days(1);
    auto days_range = std::views::iota(begin_days,end_days);
    std::ofstream missing_log(out_dir/miss_files_filename,std::ios::out);
    if(!missing_log.is_open()){
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(out_dir/miss_files_filename).c_str());
        exit((uint)ErrorCode::CANNOT_OPEN_FILE_X1);
    }
    auto initial = from;
    std::vector<fs::directory_entry> entries;
    for(const fs::directory_entry& entry: fs::directory_iterator(in_dir))
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
                threads.at(cpu) = std::move(std::thread([r,prom,&mute_at_print]() mutable{
                                    prom->set_value(process_core(std::move(r),&mute_at_print));
                                }));
            }
            for(int i = 0;i<cpus;++i){
                ProcessResult res = std::move(threads_results.at(i).get_future().get());
                if(!res.err_files.empty()){
                    std::ofstream corr_files(out_dir/errorness_files_filename,std::ios::trunc);
                    if(corr_files.is_open())
                        for(ErrorFiles& err_f:res.err_files){
                            corr_files<<err_f.name<<". Error code: "<<err_f.code<<std::endl;
                        }
                    else
                        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(out_dir/errorness_files_filename).c_str());
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
                std::ofstream err_log(out_dir/errorness_files_filename,std::ios::trunc);
                if(err_log.is_open())
                    for(ErrorFiles& err_f:res.err_files){
                        err_log<<err_f.name<<". Error code: "<<err_f.code<<std::endl;
                    }
                else
                    ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(out_dir/errorness_files_filename).c_str());
            }
            found = res.found;
        }
    }
    
    std::vector<std::chrono::sys_days> miss_data;
    std::ranges::set_difference(days_range,found,std::back_inserter(miss_data), std::ranges::less());
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

ProcessResult process_core(std::ranges::random_access_range auto&& entries, std::mutex* mute_at_print) {
    ProcessResult result;
    Check check_handler;
    for (const fs::directory_entry& entry : entries) {
        if (entry.is_regular_file() && entry.path().has_extension() && 
            (entry.path().extension() == ".grib" || entry.path().extension() == ".grb")) {
            check_handler.set_from_path(entry.path().c_str());

            // int p_line;
            // pBar bar;
            {
                if (mute_at_print){
                    std::lock_guard<std::mutex> locked(*mute_at_print);
                    p_line = progress_line++;
                    bar.setline(p_line);
                    std::cout << "\033[" << p_line<< ";0H"; // Переместить курсор на нужную строку
                    bar.update(0);
                    bar.print();
                    std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                }
                else{
                    p_line = progress_line++;
                    bar.setline(p_line);
                    std::cout << "\033[" << p_line<< ";0H"; // Переместить курсор на нужную строку
                    bar.update(0);
                    bar.print();
                    std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                }
            }
                
            check_handler.set_from_path(entry.path().c_str());
            check_handler.execute();
            check_handler.get_result().sublime();
            for(const auto& [cmn,info]:check_handler.get_result().data()){
                for(const auto& info_el:info){
                    info_el.
                    if(data.code==0)
                        result.found.insert(std::chrono::sys_days(std::chrono::year_month_day(
                            std::chrono::year(data.data.date.year),
                            std::chrono::month(data.data.date.month),
                            std::chrono::day(data.data.date.day))));
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

                    // {
                    //     if (mute_at_print){
                    //         std::lock_guard<std::mutex> locked(*mute_at_print);
                    //         bar.update((double)pos/(double)max_pos*100);
                    //         bar.print();
                    //         std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                    //     }
                    //     else {
                    //         bar.update((double)pos/(double)max_pos*100);
                    //         bar.print();
                    //         std::cout << " Thread="<<std::this_thread::get_id()<<" : "<< entry.path()<<std::flush;
                    //     }
                    // }
                }
            }
            check_handler.clear_result();
        }
        
    }
    return result;
}

bool missing_files(fs::path in_dir,fs::path out_dir, unsigned int cpus, std::chrono::year_month_day from, std::chrono::year_month_day to){
    bool result = false;
    std::set<std::chrono::sys_days> found;
    auto begin_days = std::chrono::sys_days(from);
    auto end_days = std::chrono::sys_days(to)+std::chrono::days(1);
    auto days_range = std::views::iota(begin_days,end_days);
    std::ofstream missing_log(out_dir/miss_files_filename,std::ios::out);
    if(!missing_log.is_open()){
        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(out_dir/miss_files_filename).c_str());
        exit((uint)ErrorCode::CANNOT_OPEN_FILE_X1);
    }
    auto initial = from;
    std::vector<fs::directory_entry> entries;
    for(const fs::directory_entry& entry: fs::directory_iterator(in_dir))
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
                threads.at(cpu) = std::move(std::thread([r,prom,&mute_at_print]() mutable{
                                    prom->set_value(process_core(std::move(r),&mute_at_print));
                                }));
            }
            for(int i = 0;i<cpus;++i){
                ProcessResult res = std::move(threads_results.at(i).get_future().get());
                if(!res.err_files.empty()){
                    std::ofstream corr_files(out_dir/errorness_files_filename,std::ios::trunc);
                    if(corr_files.is_open())
                        for(ErrorFiles& err_f:res.err_files){
                            corr_files<<err_f.name<<". Error code: "<<err_f.code<<std::endl;
                        }
                    else
                        ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(out_dir/errorness_files_filename).c_str());
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
                std::ofstream err_log(out_dir/errorness_files_filename,std::ios::trunc);
                if(err_log.is_open())
                    for(ErrorFiles& err_f:res.err_files){
                        err_log<<err_f.name<<". Error code: "<<err_f.code<<std::endl;
                    }
                else
                    ErrorPrint::print_error(ErrorCode::CANNOT_OPEN_FILE_X1,"",AT_ERROR_ACTION::ABORT,(out_dir/errorness_files_filename).c_str());
            }
            found = res.found;
        }
    }
    
    std::vector<std::chrono::sys_days> miss_data;
    std::ranges::set_difference(days_range,found,std::back_inserter(miss_data), std::ranges::less());
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