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
    private:
    using DataTypes = std::variant<Grib1Data>;
    std::array<DataTypes,std::variant_size_v<DataTypes>> datas_;
    std::set<Data::FORMAT> unsaved_;
    std::unordered_map<__Data__::FORMAT,fs::path> files_;
    fs::path data_directory_;

    template<Data::FORMAT>
    void __read__(const fs::path& filename);
    template<Data::FORMAT>
    void __write__(const fs::path& filename){

    }

    template <size_t I>
    void __write_all__();
    public:    
    void save();
    Data():data_directory_(fs::path(get_current_dir_name())/"data"/"bin"){}
    Data(const fs::path& data_dir):data_directory_(data_dir){}
    Data(Data&& other):
    datas_(std::move(other.datas_)),
    unsaved_(std::move(other.unsaved_)),
    files_(std::move(other.files_)),
    data_directory_(std::move(other.data_directory_)){}
    ~Data(){
        save();
    }
    void read(const fs::path& filename);
    bool write(const fs::path& filename);
    void add_data(GribDataInfo& grib_data);
    void add_data(SublimedGribDataInfo& grib_data);
    void add_data(SublimedGribDataInfo&& grib_data);
    bool unsaved() const{
        return !unsaved_.empty();
    }

    template<Data_t T,Data_f F>
    const std::unordered_set<path::Storage<true>>& paths() const{
        if(auto found = std::find(datas_.begin(),datas_.end(),[](const DataTypes& data_type){
            auto visitor = [](auto& var_data){
                if constexpr(var_data.format_type()==F && var_data.data_type()==T)
                    return true;
                else static_assert(false);
            };
        });found!=datas_.end())
            return std::get<DataStruct<T,F>>(*found).paths();
    }
    template<Data_t T,Data_f F>
    auto data_struct() const{
        if(auto found = std::find(datas_.begin(),datas_.end(),[](const DataTypes& data_type){
            auto visitor = [](auto& var_data){
                if constexpr(var_data.format_type()==F && var_data.data_type()==T)
                    return true;
                else static_assert(false);
            };
        });found!=datas_.end())
            return std::get<DataStruct<T,F>>(*found);
    }
    const std::unordered_map<Data_f,fs::path>& written_files() const{
        return files_;
    }
    template<Data_t TYPE,Data_f FORMAT>
    const SublimedDataInfoStruct<TYPE,FORMAT>& sublimed_data() const{
        return data_struct<TYPE,FORMAT>().sublimed_;
    }

    template<Data_t T,Data_f F,typename... ARGS>
    std::unordered_map<path::Storage<true>,SublimedDataInfoStruct<T,F>> match_data(
        ARGS&&... args
    ) const{
        if(auto found = std::find(datas_.begin(),datas_.end(),[](const DataTypes& data_type){
            auto visitor = [](auto& var_data){
                if constexpr(var_data.format_type()==F && var_data.data_type()==T)
                    return true;
                else static_assert(false);
            };
            return std::visit(visitor,data_type);
        });found!=datas_.end())
            return std::get<DataStruct<T,F>>(*found).match_data(std::forward<ARGS>(args)...);
    }

    template<Data_t T,Data_f F,typename RESULT,typename... ARGS>
    RESULT match(
        ARGS&&... args
    ) const{
        if(auto found = std::find(datas_.begin(),datas_.end(),[](const DataTypes& data_type){
            auto visitor = [](auto& var_data){
                if constexpr(var_data.format_type()==F && var_data.data_type()==T)
                    return true;
                else static_assert(false);
            };
            return std::visit(visitor,data_type);
        });found!=datas_.end())
            return std::get<DataStruct<T,F>>(*found).match(std::forward<ARGS>(args)...);
    }

    template<Data_t T,Data_f F,typename RESULT,typename... ARGS>
    RESULT find_all(
        ARGS&&... args
    ) const{
        if(auto found = std::find(datas_.begin(),datas_.end(),[](const DataTypes& data_type){
            auto visitor = [](auto& var_data){
                if constexpr(var_data.format_type()==F && var_data.data_type()==T)
                    return true;
                else static_assert(false);
            };
            return std::visit(visitor,data_type);
        });found!=datas_.end())
            return std::get<DataStruct<T,F>>(*found).find_all(std::forward<ARGS>(args)...);
    }
};