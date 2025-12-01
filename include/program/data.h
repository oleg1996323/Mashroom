#pragma once
#include "data/sublimed_info.h"
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <unordered_set>
#include <map>
#include <set>
#include "data/info.h"
#include "data/def.h"
#include "sys/error_print.h"
#include "types/time_interval.h"
#include "definitions/def.h"
#include "definitions/path_process.h"
#include <cstdint>
#include "data/datastruct.h"

using namespace std::chrono;
using namespace std::string_literals;

namespace fs = std::filesystem;
class Data:public __Data__{
    protected:
    mutable std::unordered_set<std::unique_ptr<AbstractDataStruct>> datas_;
    std::set<Data_f> unsaved_;
    std::unordered_map<__Data__::FORMAT,fs::path> files_;
    fs::path data_directory_;

    friend class DataTestClass;
    template<Data_f>
    ErrorCode __read__(const fs::path& filename);
    template<Data_f>
    ErrorCode __write__(const fs::path& filename);

    template <Data_f I>
    void __write_all__();
    //@todo make private
    public:
    template<Data_t T,Data_f F>
    DataStruct<T,F>& data_struct(){
        decltype(datas_)::const_iterator found = datas_.find(std::make_pair<Data_f,Data_t>(F,T));
        if(found==datas_.end())
            found = datas_.insert(std::move(std::make_unique<DataStruct<T,F>>())).first;
        return *dynamic_cast<DataStruct<T,F>*>(found->get());
    }
    public:    
    void save();
    Data():data_directory_(fs::path(get_current_dir_name())/"data"/"bin"){}
    Data(const fs::path& data_dir):data_directory_(data_dir){}
    Data(const Data&) = delete;
    Data(Data&& other):
    datas_(std::move(other.datas_)),
    unsaved_(std::move(other.unsaved_)),
    files_(std::move(other.files_)),
    data_directory_(std::move(other.data_directory_)){}
    ~Data(){
        save();
    }
    ErrorCode read(const fs::path& filename) noexcept;
    ErrorCode write(const fs::path& filename) noexcept;
    bool unsaved() const{
        return !unsaved_.empty();
    }

    template<Data_t T,Data_f F>
    const std::unordered_set<path::Storage<true>>& paths() const{
        return data_struct<T,F>().paths();
    }
    template<Data_t T,Data_f F>
    const DataStruct<T,F>& data_struct() const{
        decltype(datas_)::const_iterator found = datas_.find(std::make_pair<Data_f,Data_t>(F,T));
        if(found!=datas_.end())
            return *dynamic_cast<const DataStruct<T,F>*>(found->get());
        else
            return *dynamic_cast<const DataStruct<T,F>*>(datas_.insert(std::make_unique<DataStruct<T,F>>()).first->get());
    }
    const std::unordered_map<Data_f,fs::path>& written_files() const{
        return files_;
    }
    template<Data_t TYPE,Data_f FORMAT>
    const SublimedFormatDataInfo<TYPE,FORMAT>& sublimed_data() const{
        return data_struct<TYPE,FORMAT>().sublimed_;
    }

    template<Data_t T,Data_f F,typename... ARGS>
    typename DataStruct<T,F>::match_data_t match_data(
        ARGS&&... args
    ) const{
        return data_struct<T,F>().match_data(std::forward<ARGS>(args)...);
    }

    template<Data_t T,Data_f F,typename... ARGS>
    typename DataStruct<T,F>::match_t match(ARGS&&... args){
        return data_struct<T,F>(std::forward<ARGS>(args)...);
    }

    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::match_data_t match_data(
        Organization center,
        std::optional<TimeFrame> time_fcst,
        const std::unordered_set<SearchParamTableVersion>& parameters,
        TimeInterval time_interval,
        RepresentationType rep_t,
        Coord pos
    ){
        return data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().match_data(center,time_fcst,parameters,time_interval,rep_t,pos);
    }

    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::match_t match(
        path::Storage<true> path,
        Organization center,
        std::optional<TimeFrame> timeframe,
        const std::unordered_set<SearchParamTableVersion>& param_tables,
        TimeInterval t_interval,
        RepresentationType grid_type,
        Coord pos) const{
        return data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().match(
            path,center,timeframe,param_tables,t_interval,grid_type,pos);
    }

    template<Data_t T,Data_f F,typename... ARGS>
    FoundSublimedDataInfo<T,F> find_all(
        ARGS&&... args
    ) const{
        return data_struct<T,F>().find_all(std::forward<ARGS>(args)...);
    }

    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::find_all_t find_all(std::optional<RepresentationType> grid_type_,
    std::optional<TimeSequence> time_,
    std::optional<TimeFrame> forecast_preference_,
    utc_tp last_update_) const{
        return data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().find_all(grid_type_,time_,forecast_preference_,last_update_);
    }

    template<Data_t T, Data_f F>
    void add_data(SublimedFormatDataInfo<T,F>& data){
        data_struct<T,F>().add_data(data);
        unsaved_.insert(F);
        std::cout<<"Unsaved files: "<<unsaved_.size()<<std::endl;
    }
    template<Data_t T, Data_f F>
    void add_data(SublimedFormatDataInfo<T,F>&& data){
        data_struct<T,F>().add_data(std::move(data));
        unsaved_.insert(F);
        std::cout<<"Unsaved files: "<<unsaved_.size()<<std::endl;
    }
};

template<Data_t T,Data_f F>
struct DataMethodType{
    template<typename... ARGS>
    using match_t = std::invoke_result_t<decltype(&Data::match<T,F>),
                                        Data*,
                                        ARGS...>;
    template<typename... ARGS>
    using match_data_t = std::invoke_result_t<decltype(&Data::match_data<T,F>),
                                        Data*,
                                        ARGS...>;
    template<typename... ARGS>
    using find_all_t = std::invoke_result_t<decltype(&Data::find_all<T,F>),
                                        Data*,
                                        ARGS...>;
};