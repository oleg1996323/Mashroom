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


namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,DataStruct<Data_t::METEO,Data_f::GRIB>>{
        using type = DataStruct<Data_t::METEO,Data_f::GRIB>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.sublimed_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,DataStruct<Data_t::METEO,Data_f::GRIB>>{
        using type = DataStruct<Data_t::METEO,Data_f::GRIB>;
        SerializationEC operator()(type& msg, std::span<const char> buf) const noexcept{
            if(auto err = deserialize<NETWORK_ORDER>(msg,buf,msg.sublimed_);err!=SerializationEC::NONE)
                return err;
            else{
                for(auto& [filename,file_data]:msg.sublimed_.data()){
                    for(auto& [common,grib_data]:file_data){
                        msg.by_common_data_[common].insert(filename);
                        for(const auto& sub_data:grib_data){
                            msg.by_date_[sub_data.sequence_time_].insert(filename);
                            msg.by_grid_[sub_data.grid_data_].insert(filename);
                        }
                    }
                }
                return SerializationEC::NONE;
            }
        }
    };

    template<>
    struct Serial_size<DataStruct<Data_t::METEO,Data_f::GRIB>>{
        using type = DataStruct<Data_t::METEO,Data_f::GRIB>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.sublimed_);
        }
    };

    template<>
    struct Min_serial_size<DataStruct<Data_t::METEO,Data_f::GRIB>>{
        using type = DataStruct<Data_t::METEO,Data_f::GRIB>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::sublimed_)>();
        }();
    };

    template<>
    struct Max_serial_size<DataStruct<Data_t::METEO,Data_f::GRIB>>{
        using type = DataStruct<Data_t::METEO,Data_f::GRIB>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::sublimed_)>();
        }();
    };
}