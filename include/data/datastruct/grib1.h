#pragma once
#include "datastructdef.h"

using Grib1Data = DataStruct<Data_t::METEO,Data_f::GRIB>;

template<>
struct DataStruct<Data_t::METEO,Data_f::GRIB>:public AbstractDataStruct{
    using sublimed_data_by_common_data = std::unordered_map<std::weak_ptr<Grib1CommonDataProperties>,std::unordered_set<path::Storage<true>>>;
    using sublimed_data_by_date_time = std::map<TimeSequence,std::unordered_set<path::Storage<true>>>;
    using sublimed_data_by_grid = std::unordered_map<std::optional<GridInfo>,std::unordered_set<path::Storage<true>>>;
    SublimedGribDataInfo sublimed_;
    sublimed_data_by_common_data by_common_data_;
    sublimed_data_by_date_time by_date_;
    sublimed_data_by_grid by_grid_;
    DataStruct() = default;
    DataStruct(const DataStruct&) = delete;
    DataStruct(DataStruct&& other):
    sublimed_(std::move(other.sublimed_)),
    by_common_data_(std::move(other.by_common_data_)),
    by_date_(std::move(other.by_date_)),
    by_grid_(std::move(other.by_grid_)){}

    constexpr virtual Data_f format_type() const noexcept override{
        return Data_f::GRIB;
    }
    constexpr virtual Data_t data_type() const noexcept override{
        return Data_t::METEO;
    }

    const std::unordered_set<path::Storage<true>>& paths() const{
        return sublimed_.paths();
    }

    void add_data(SublimedGribDataInfo& grib_data);
    void add_data(SublimedGribDataInfo&& grib_data);

    std::unordered_map<path::Storage<true>,GribSublimedDataInfoStruct> match_data(
        Organization center,
        std::optional<TimeFrame> time_fcst,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        TimeInterval time_interval,
        RepresentationType rep_t,
        Coord pos
    ) const;

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