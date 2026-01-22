#include "data/datastruct/grib1.h"

// void Grib1Data::add_data(SublimedGribDataInfo& grib_data){
//     if(grib_data.data().empty()){
//         std::cout<<"Nothing changes"<<std::endl;
//         return;
//     }
//     sublimed_.add_data(grib_data);
//     for(auto& [filename,file_data]:grib_data.data()){
//         const auto& tmp_view = sublimed_.data().find(filename)->first;
//         for(auto& [common,grib_data]:sublimed_.data().find(filename)->second){
//             by_common_data_[common].insert(tmp_view);
//             by_forecast_[common].insert(tmp_view);
//             by_level_[common].insert(tmp_view);
//             for(const auto& sub_data:grib_data){
//                 by_date_[sub_data.sequence_time_].insert(tmp_view);
//                 by_grid_[sub_data.grid_data_].insert(tmp_view);
//             }
//         }
//     }
// }
// void Grib1Data::add_data(SublimedGribDataInfo&& grib_data){
//     if(grib_data.data().empty()){
//         std::cout<<"Nothing changes"<<std::endl;
//         return;
//     }
//     sublimed_.add_data(grib_data);
//     for(auto& [filename,file_data]:grib_data.data()){
//         const auto& tmp_view = sublimed_.data().find(filename)->first;
//         for(auto& [common,grib_data]:sublimed_.data().find(filename)->second){
//             by_common_data_[common].insert(tmp_view);
//             by_forecast_[common].insert(tmp_view);
//             by_level_[common].insert(tmp_view);
//             for(const auto& sub_data:grib_data){
//                 by_date_[sub_data.sequence_time_].insert(tmp_view);
//                 by_grid_[sub_data.grid_data_].insert(tmp_view);
//             }
//         }
//     }
// }

//match data and return files and corresponding sublimed data
std::unordered_map<path::Storage<true>,std::vector<ptrdiff_t>> Grib1Data::match_data(
    Organization center,
    std::optional<TimeForecast> time_fcst,
    std::optional<Level> level_,
    const std::unordered_set<SearchParamTableVersion>& parameters,
    TimeInterval time_interval,
    RepresentationType rep_t,
    Coord pos
) const{
    std::unordered_map<path::Storage<true>,std::span<ptrdiff_t>> result;
    // std::unordered_set<Grib1CommonDataProperties> param_variations=get_parameter_variations(center,time_fcst,
    //                                         level_,parameters);
    // for(const auto& common:param_variations){
    //     auto found = by_common_data_.find(common);
    //     if(found!=by_common_data_.end()){
    //         for(auto [fn,ptrs]:found->second)
    //             result[fn]=std::span(ptrs);
    //     }
    // }
    // for(const auto& [date_interval,seq_fn]:by_date_){
    //     if(!intervals_intersect(time_interval,date_interval.get_interval()))
    //         for(auto fn:seq_fn){
    //             result.erase(fn);
    //         }
    // }
    // for(const auto& [grid,seq_fn]:by_grid_){
    //     if(!grid.has_value())
    //         for(auto fn:seq_fn){
    //             result.erase(fn);
    //         }
    //     if(!pos_in_grid(pos,grid.value()))
    //         for(auto fn:seq_fn){
    //             result.erase(fn);
    //         }
    // }
    // for(auto& [fn,ptrs]:result){
    //     std::cout<<fn<<" parameters:"<<std::flush;
    //     for(const auto& [common,data]:sublimed_.data().find(fn)->second)
    //         if(param_variations.contains(*common))
    //             for(const auto& d:data){
    //                 if(d.grid_data_.has_value() && 
    //                     d.grid_data_.value().type()==rep_t && 
    //                     intervals_intersect(d.sequence_time_.get_interval().from(),d.sequence_time_.get_interval().to(),time_interval.from(),time_interval.to()) && 
    //                     pos_in_grid(pos,d.grid_data_.value()))
    //                 {
    //                     std::cout<<" "<<std::string_view(parameter_table(common->center_.value(),common->table_version_.value(),common->parameter_.value())->name)<<std::flush;
    //                     std::error_code err;
    //                     auto beg_end = interval_intersection_pos(time_interval,TimeSequence(d.sequence_time_.get_interval().from(),d.sequence_time_.get_interval().to(),d.sequence_time_.time_duration(),err),err);
    //                     if(err!=std::error_code())
    //                         continue;//@todo
    //                     if(beg_end.has_value())
    //                         ptrs.buf_pos_.append_range(d.buf_pos_|std::views::drop(beg_end->first)|std::views::take(beg_end->second-beg_end->first+1));
    //                     auto interseq_interval = interval_intersection(d.sequence_time_.get_interval(),time_interval);
    //                     assert(interseq_interval.has_value());
    //                     ptrs.sequence_time_ = std::move(TimeSequence(interseq_interval->from(),interseq_interval->to(),d.sequence_time_.time_duration(),err));
    //                     ptrs.grid_data_ = d.grid_data_;
    //                 }
    //             }
    //     std::cout<<std::endl;
    // }
    // return result;
}

//match data by specified file
std::vector<ptrdiff_t> Grib1Data::match(
        path::Storage<true> path,
        Organization center,
        std::optional<TimeForecast> time_fcst,
        std::optional<Level> level_,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        TimeInterval time_interval,
        RepresentationType rep_t,
        Coord pos
    ) const
{
    std::vector<ptrdiff_t> result;
    // std::unordered_set<Grib1CommonDataProperties> param_variations=get_parameter_variations(center,time_fcst,
    //                                         level_,parameters);
    // if(!sublimed_.data().contains(path))
    //     return result;
    // auto& file_data = sublimed_.data().at(path);
    // for(const auto& common:param_variations){
    //     auto found = file_data.find(common);
    //     if(found!=file_data.end()){
    //         for(auto info:found->second){
    //             if(info.grid_data_.has_value() &&
    //                 info.grid_data_.value().type()==rep_t &&
    //                 pos_in_grid(pos,info.grid_data_.value()) &&
    //                 intervals_intersect(info.sequence_time_.get_interval(),time_interval))
    //             {
    //                 std::error_code err;
    //                 auto beg_end = interval_intersection_pos(time_interval,TimeSequence(info.sequence_time_.get_interval().from(),info.sequence_time_.get_interval().to(),info.sequence_time_.time_duration(),err),err);
    //                 if(beg_end.has_value() && err==std::error_code())
    //                     result.append_range(std::move(info.buf_pos_|std::views::drop(beg_end->first)|std::views::take(beg_end->second-beg_end->first+1)));
    //             }
    //         }
    //     }
    // }
    // std::sort(result.begin(),result.end());
    // return result;
}

std::vector<FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>> Grib1Data::find_all(std::optional<RepresentationType> grid_type_,
                std::optional<TimeInterval> tinterval_,
                std::optional<DateTimeDiff> tdiff_,
                std::optional<TimeForecast> forecast_preference_,
                std::optional<Level> level_,
                std::optional<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>> common_,
                utc_tp last_update_) const
{

    std::vector<FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1>> result;
    auto check_update = [last_update_](const auto& add_path){
        using type = std::decay_t<decltype(add_path)>;
        if constexpr(std::is_same_v<type,std::monostate>)
            return false;
        else return add_path.last_check_<last_update_;
    }; 
    for(auto& index_val:index_){
        if(!std::visit(check_update,index_val.path_.add_))
            continue;
        if(grid_type_.has_value() && grid_type_.value()!=index_val.grid_->type())
            continue;
        if(tdiff_.has_value() && tdiff_.value()!=index_val.ts_.time_duration())
            continue;
        if(common_.has_value() && common_.value()!=index_val.cmn_)
            continue;
        if(forecast_preference_.has_value() && forecast_preference_.value()!=index_val.tf_)
            continue;
        if(level_.has_value() && level_.value()!=index_val.lvl_)
            continue;
        TimeInterval tinterval_to_add;
        if(tinterval_.has_value() && !intervals_intersect(tinterval_.value(),index_val.ts_.get_interval()))
            continue;
        else tinterval_to_add = interval_intersection(tinterval_.value(),index_val.ts_.get_interval()).value();

        FoundSublimedDataInfo<Data_t::TIME_SERIES,Data_f::GRIB_v1> to_add;
        to_add.cmn_=index_val.cmn_;
        to_add.add_.fcst_=index_val.tf_;
        to_add.add_.lvl_=index_val.lvl_;
        to_add.add_.grid_=index_val.grid_;
        std::error_code err;
        to_add.add_.ts_=TimeSequence(tinterval_to_add.from(),tinterval_to_add.to(),index_val.ts_.time_duration(),err);
        if(err!=std::error_code())
            continue;
        else result.push_back(to_add);
    }
}

//@todo make search by forecast, levels etc
std::unordered_set<Grib1CommonDataProperties> Grib1Data::get_parameter_variations(Organization center,
    std::optional<TimeForecast> time_fcst,
    std::optional<Level> level_,
    const std::unordered_set<SearchParamTableVersion>& parameters) noexcept
{
    std::unordered_set<Grib1CommonDataProperties> result;
    // if(!time_fcst.has_value()){
    //     int time_counter = 0;
    //     for(int time=0;time<256;++time)
    //         if(is_time[time]){
    //             if(parameters.empty()){
    //                 for(int param=0;param<256;++param){
    //                     for(int table_version=0;table_version<256;++table_version)
    //                         result.insert(Grib1CommonDataProperties(center,table_version,std::nullopt,param,level_));
    //                 }
    //             }
    //             else{
    //                 int param_counter = 0;
    //                 for(const auto& [param,table_version]:parameters)
    //                     result.insert(Grib1CommonDataProperties(center,table_version,std::nullopt,param,level_));
    //             }
    //             ++time_counter;
    //         }
    //         else continue;
    // }
    // else{
    //     uint8_t param_counter = 0;
    //     for(const auto& [param,table_version]:parameters)
    //         result.insert(Grib1CommonDataProperties(center,table_version,time_fcst.value(),param,level_));
    // }
    return result;
}