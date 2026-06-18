#pragma once
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <unordered_set>
#include <map>
#include <set>
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
    mutable std::unordered_set<DataStructVariation> datas_;
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
        if(found!=datas_.end() && std::holds_alternative<DataStruct<T,F>>(*found))
            return const_cast<DataStruct<T,F>&>(std::get<DataStruct<T,F>>(*found));
        else{
            DataStructVariation var;
            var.emplace<DataStruct<T,F>>();
            return const_cast<DataStruct<T,F>&>(std::get<DataStruct<T,F>>(*datas_.insert(std::move(var)).first));
        }
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
        if(found!=datas_.end() && std::holds_alternative<DataStruct<T,F>>(*found))
            return std::get<DataStruct<T,F>>(*found);
        else{
            DataStructVariation var;
            var.emplace<DataStruct<T,F>>();
            return std::get<DataStruct<T,F>>(*datas_.insert(std::move(var)).first);
        }
    }
    const std::unordered_map<Data_f,fs::path>& written_files() const{
        return files_;
    }

    template<Data_t T,Data_f F,typename... ARGS>
    typename DataStruct<T,F>::match_files_t match_files(
        ARGS&&... args
    ) const{
        return data_struct<T,F>().match_files(std::forward<ARGS>(args)...);
    }

    template<Data_t T,Data_f F,typename... ARGS>
    typename DataStruct<T,F>::match_t match(ARGS&&... args){
        return data_struct<T,F>(std::forward<ARGS>(args)...);
    }

    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::match_files_t match_files(
        utc_tp last_update,
        Coord pos,
        Organization center,
        const std::unordered_set<SearchParamTableVersion>& param_tables,
        std::optional<utc_tp_t<std::chrono::seconds>> from,
        std::optional<utc_tp_t<std::chrono::seconds>> to,
        std::optional<DateTimeDiff> diff,
        std::optional<TimeForecast> forecast_preferences,
        std::optional<Level> level_,
        std::optional<RepresentationType> rep_t
        ) const{
        return data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().match_files(
            last_update,pos,center,param_tables,from,to,diff,forecast_preferences,
            level_,rep_t);
    }

    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::match_t match(
        std::string_view path,
        utc_tp last_update,
        Coord pos,
        Organization center,
        const std::unordered_set<SearchParamTableVersion>& param_tables,
        std::optional<utc_tp_t<std::chrono::seconds>> from,
        std::optional<utc_tp_t<std::chrono::seconds>> to,
        std::optional<DateTimeDiff> diff,
        std::optional<TimeForecast> forecast_preferences,
        std::optional<Level> level_,
        std::optional<RepresentationType> rep_t
        ) const{
        return data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().match(
            path,last_update,pos,center,param_tables,from,to,diff,forecast_preferences,
            level_,rep_t);
    }

    template<Data_t T,Data_f F,typename... ARGS>
    std::vector<SearchDataResult<T,F>> find_all(
        ARGS&&... args
    ) const{
        return data_struct<T,F>().find_all(std::forward<ARGS>(args)...);
    }

    DataStruct<Data_t::TIME_SERIES,Data_f::GRIB_v1>::find_all_t find_all(
        const std::unordered_set<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>>& cmn,
        utc_tp last_update,
        std::optional<Lat> top,
        std::optional<Lat> bottom,
        std::optional<Lon> left,
        std::optional<Lon> right,
        std::optional<utc_tp_t<std::chrono::seconds>> from,
        std::optional<utc_tp_t<std::chrono::seconds>> to,
        std::optional<DateTimeDiff> tdiff,
        std::optional<std::pair<TimeForecast,TimeForecast::COMPARISION_TYPE>> forecast_preference,
        std::optional<std::pair<Level,Level::COMPARISION_TYPE>> level,
        std::optional<RepresentationType> grid_type) const{
            return data_struct<Data_t::TIME_SERIES,Data_f::GRIB_v1>().
            find_all(cmn,last_update,top,bottom,left,right,
                from,to,tdiff,forecast_preference,level,grid_type);
    }

    template<Data_t T, Data_f F>
    void update_indexing(const DataStruct<T,F>& data){
        data_struct<T,F>().update_indexing(data);
        unsaved_.insert(F);
        std::cout<<"Unsaved files: "<<unsaved_.size()<<std::endl;
    }
    template<Data_t T, Data_f F>
    void update_indexing(DataStruct<T,F>&& data){
        data_struct<T,F>().update_indexing(std::move(data));
        unsaved_.insert(F);
        std::cout<<"Unsaved files: "<<unsaved_.size()<<std::endl;
    }
    template<Data_t T, Data_f F>
    void add_data(const path::Storage<false>& path,const std::vector<data::FileMsg<T,F>>& data){
        std::error_code err;
        data_struct<T,F>().add_data(path,data,err);
        if(err==std::error_code()){
            unsaved_.insert(F);
            std::cout<<"Unsaved files: "<<unsaved_.size()<<std::endl;
        }
    }
};

template<Data_t T,Data_f F>
struct DataMethodType{
    template<typename... ARGS>
    using match_files_t = std::invoke_result_t<decltype(&Data::match_files<T,F>),
                                        Data*,
                                        ARGS...>;
    template<typename... ARGS>
    using match_t = std::invoke_result_t<decltype(&Data::match<T,F>),
                                        Data*,
                                        ARGS...>;
    template<typename... ARGS>
    using find_all_t = std::invoke_result_t<decltype(&Data::find_all<T,F>),
                                        Data*,
                                        ARGS...>;
};