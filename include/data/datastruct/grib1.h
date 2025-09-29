#pragma once
#include "datastructdef.h"

using Grib1Data = DataStruct<Data_t::METEO,Data_f::GRIB>;

template<>
struct DataStruct<Data_t::METEO,Data_f::GRIB>:__Data_type__<Data_t::METEO,Data_f::GRIB>{
    using sublimed_data_by_common_data = std::unordered_map<std::weak_ptr<Grib1CommonDataProperties>,std::unordered_set<path::Storage<true>>>;
    using sublimed_data_by_date_time = std::map<TimeSequence,std::unordered_set<path::Storage<true>>>;
    using sublimed_data_by_grid = std::unordered_map<std::optional<GridInfo>,std::unordered_set<path::Storage<true>>>;
    SublimedGribDataInfo sublimed_;
    sublimed_data_by_common_data by_common_data_;
    sublimed_data_by_date_time by_date_;
    sublimed_data_by_grid by_grid_;
    std::unique_ptr<DataStruct> unsaved_;
    DataStruct() = default;
    DataStruct(const DataStruct&) = delete;
    DataStruct(DataStruct&& other):
    sublimed_(std::move(other.sublimed_)),
    by_common_data_(std::move(other.by_common_data_)),
    by_date_(std::move(other.by_date_)),
    by_grid_(std::move(other.by_grid_)){}

    auto match_data(
        Organization center,
        std::optional<TimeFrame> time_fcst,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        TimeInterval time_interval,
        RepresentationType rep_t,
        Coord pos
    ) const -> std::unordered_map<path::Storage<true>,GribSublimedDataInfoStruct>;

    std::vector<ptrdiff_t> match(
        path::Storage<true> path,
        Organization center,
        std::optional<TimeFrame> time_fcst,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        TimeInterval time_interval,
        RepresentationType rep_t,
        Coord pos
    ) const;

    FoundSublimedDataInfo<Data_t::METEO,Data_f::GRIB> find_all(std::optional<RepresentationType> grid_type_,
                        std::optional<TimeSequence> time_,
                        std::optional<TimeFrame> forecast_preference_,
                        utc_tp last_update_) const;

    static std::unordered_set<Grib1CommonDataProperties> get_parameter_variations(Organization center,
    std::optional<TimeFrame> time_fcst,
    const std::unordered_set<SearchParamTableVersion>& parameters) noexcept;

    using match_data_t = std::invoke_result_t<  decltype(&DataStruct::match_data),
                                                    DataStruct*,
                                                    Organization,
                                                    std::optional<TimeFrame>,
                                                    const std::unordered_set<SearchParamTableVersion>&,
                                                    TimeInterval,
                                                    RepresentationType,
                                                    Coord>;
    using match_t = std::invoke_result_t<           decltype(&DataStruct::match),
                                                    DataStruct*,
                                                    path::Storage<true>,
                                                    Organization,
                                                    std::optional<TimeFrame>,
                                                    const std::unordered_set<SearchParamTableVersion>&,
                                                    TimeInterval,
                                                    RepresentationType,
                                                    Coord>;
    using find_all_t = std::invoke_result_t<      decltype(&DataStruct::find_all),
                                                    DataStruct*,
                                                    std::optional<RepresentationType>,
                                                    std::optional<TimeSequence>,
                                                    std::optional<TimeFrame>,
                                                    utc_tp>;
};


std::unordered_map<path::Storage<true>,GribSublimedDataInfoStruct> Grib1Data::match_data(
    Organization center,
    std::optional<TimeFrame> time_fcst,
    const std::unordered_set<SearchParamTableVersion>& parameters,
    TimeInterval time_interval,
    RepresentationType rep_t,
    Coord pos
) const{
    std::unordered_map<path::Storage<true>,GribSublimedDataInfoStruct> result;
    std::unordered_set<Grib1CommonDataProperties> param_variations=get_parameter_variations(center,time_fcst,parameters);
    for(const auto& common:param_variations){
        auto found = by_common_data_.find(common);
        if(found!=by_common_data_.end() && !found->first.expired()){
            for(auto fn:found->second)
                result[fn];
        }
    }
    for(const auto& [date_interval,seq_fn]:by_date_){
        if(!intervals_intersect(time_interval,date_interval.interval_))
            for(auto fn:seq_fn){
                result.erase(fn);
            }
    }
    for(const auto& [grid,seq_fn]:by_grid_){
        if(!grid.has_value())
            for(auto fn:seq_fn){
                result.erase(fn);
            }
        if(!pos_in_grid(pos,grid.value()))
            for(auto fn:seq_fn){
                result.erase(fn);
            }
    }
    for(auto& [fn,ptrs]:result){
        std::cout<<fn<<" parameters:"<<std::flush;
        for(const auto& [common,data]:sublimed_.data().find(fn)->second)
            if(param_variations.contains(*common))
                for(const auto& d:data){
                    if(d.grid_data_.has_value() && 
                        d.grid_data_.value().type()==rep_t && 
                        intervals_intersect(d.sequence_time_.interval_.from_,d.sequence_time_.interval_.to_,time_interval.from_,time_interval.to_) && 
                        pos_in_grid(pos,d.grid_data_.value()))
                    {
                        std::cout<<" "<<std::string_view(parameter_table(common->center_.value(),common->table_version_.value(),common->parameter_.value())->name)<<std::flush;
                        auto beg_end = interval_intersection_pos(time_interval,d.sequence_time_.interval_,d.sequence_time_.discret_);
                        ptrs.buf_pos_.append_range(d.buf_pos_|std::views::drop(beg_end.first)|std::views::take(beg_end.second-beg_end.first));
                        ptrs.sequence_time_.interval_.from_ = d.sequence_time_.interval_.from_;
                        ptrs.sequence_time_.interval_.to_ = d.sequence_time_.interval_.to_;
                        ptrs.sequence_time_.discret_ = d.sequence_time_.discret_;
                        ptrs.grid_data_ = d.grid_data_;
                    }
                }
        std::cout<<std::endl;
    }
    return result;
}

FoundSublimedDataInfo<Data_t::METEO,Data_f::GRIB> Grib1Data::find_all(std::optional<RepresentationType> grid_type_,
                std::optional<TimeSequence> time_,
                std::optional<TimeFrame> forecast_preference_,
                utc_tp last_update_) const{
    FoundSublimedDataInfo<Data_t::METEO,Data_f::GRIB> result;
    for(const auto& [path,dat]:this->sublimed_.data()){
        if(path.type_==path::TYPE::FILE && path.add_.get<path::TYPE::FILE>().last_check_>=last_update_){
            Grib1CommonDataProperties searched(std::nullopt,std::nullopt,forecast_preference_,std::nullopt);
            if(auto begin = dat.lower_bound(searched); begin!=dat.end()){
                for(const auto& [cmn,sublimed]:std::ranges::subrange(begin,dat.upper_bound(searched))){
                    if(time_){
                        for(auto& sub:sublimed){
                            if( auto instersection = interval_instersection(time_->interval_,sub.sequence_time_.interval_);
                                instersection.has_value())
                                result[*cmn].push_back(sub);
                            else continue;
                        }
                    }
                    else result[*cmn].insert_range(result[*cmn].begin(),sublimed);
                }
            }
            else continue;
        }
        else continue;
    }
    return result;
}

std::unordered_set<Grib1CommonDataProperties> Grib1Data::get_parameter_variations(Organization center,
    std::optional<TimeFrame> time_fcst,
    const std::unordered_set<SearchParamTableVersion>& parameters) noexcept
{
    std::unordered_set<Grib1CommonDataProperties> result;
    if(!time_fcst.has_value()){
        int time_counter = 0;
        for(int time=0;time<256;++time)
            if(is_time[time]){
                if(parameters.empty()){
                    for(int param=0;param<256;++param){
                        for(int table_version=0;table_version<256;++table_version)
                            result.insert(Grib1CommonDataProperties(center,table_version,(TimeFrame)time,param));
                    }
                }
                else{
                    int param_counter = 0;
                    for(const auto& [param,table_version]:parameters)
                        result.insert(Grib1CommonDataProperties(center,table_version,(TimeFrame)time,param));
                }
                ++time_counter;
            }
            else continue;
    }
    else{
            if(is_time[time_fcst.value()] && parameters.empty()){
                for(uint8_t param=0;param<256;++param)
                    for(int table_version=0;table_version<256;++table_version)
                        result.insert(Grib1CommonDataProperties(center,table_version,time_fcst.value(),param));
            }
            else{
                uint8_t param_counter = 0;
                for(const auto& [param,table_version]:parameters)
                    result.insert(Grib1CommonDataProperties(center,table_version,time_fcst.value(),param));
            }
    }
    return result;
}