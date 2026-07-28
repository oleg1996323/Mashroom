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
#include "OsterLib/types/time_interval.h"
#include "definitions/def.h"
#include "Location.h"
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
    mashroom::errc __read__(const fs::path& filename);
    template<Data_f>
    mashroom::errc __write__(const fs::path& filename);

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
    mashroom::errc read(const fs::path& filename) noexcept;
    mashroom::errc write(const fs::path& filename) noexcept;
    bool unsaved() const{
        return !unsaved_.empty();
    }

    template<Data_t T,Data_f F>
    const std::unordered_set<Location<true>>& paths() const{
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
    void add_data(const Location<false>& path,const std::vector<data::FileMsg<T,F>>& data){
        std::error_code err;
        data_struct<T,F>().add_data(path,data,err);
        if(err==std::error_code()){
            unsaved_.insert(F);
            std::cout<<"Unsaved files: "<<unsaved_.size()<<std::endl;
        }
    }
};