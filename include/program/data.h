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
    std::unordered_set<std::unique_ptr<AbstractDataStruct>> datas_;
    std::set<Data_f> unsaved_;
    std::unordered_map<__Data__::FORMAT,fs::path> files_;
    fs::path data_directory_;

    friend class DataTestClass;
    template<Data::FORMAT>
    void __read__(const fs::path& filename);
    template<Data::FORMAT>
    void __write__(const fs::path& filename){
        
    }

    template <size_t I>
    void __write_all__();
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
    void read(const fs::path& filename);
    bool write(const fs::path& filename);
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
        assert(found!=datas_.end());
        return *dynamic_cast<const DataStruct<T,F>*>(found->get());
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
    typename DataStruct<T,F>::match_t match(
        ARGS&&... args
    ) const{
        return data_struct<T,F>().match(std::forward<ARGS>(args)...);
    }

    template<Data_t T,Data_f F,typename... ARGS>
    FoundSublimedDataInfo<T,F> find_all(
        ARGS&&... args
    ) const{
        return data_struct<T,F>().find_all(std::forward<ARGS>(args)...);
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