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
#include "data/sublimed_info.h"
#include "sys/error_print.h"
#include "types/time_interval.h"
#include "cmd_parse/cmd_translator.h"
#include "definitions/def.h"
#include "definitions/path_process.h"

using namespace std::chrono;
using namespace std::string_literals;

namespace fs = std::filesystem;
class Data:public __Data__{
    public:
        using sublimed_data_by_common_data = std::unordered_map<std::weak_ptr<CommonDataProperties>,std::unordered_set<path::Storage<true>>>;
        using sublimed_data_by_date_time = std::map<TimeInterval,std::unordered_set<path::Storage<true>>>;
        using sublimed_data_by_grid = std::unordered_map<std::optional<GridInfo>,std::unordered_set<path::Storage<true>>>;
        enum class TYPE{
            METEO,
            TOPO,
            KADASTR
        };
        enum class ACCESS{
            PUBLIC,
            PROTECTED,
            PRIVATE
        };
    
    private:
    template<TYPE DATA_T,FORMAT DATA_F>
    struct __Data_type__{
        static constexpr uint8_t format_t = DATA_F;
        static constexpr uint8_t data_t = DATA_T;
    };

    struct GribData:__Data_type__<TYPE::METEO,FORMAT::GRIB>{
        SublimedGribDataInfo grib_data_;
        sublimed_data_by_common_data by_common_data_;
        sublimed_data_by_date_time by_date_;
        sublimed_data_by_grid by_grid_;
        std::unique_ptr<GribData> unsaved_;
        GribData() = default;
        GribData(const GribData&) = delete;
        GribData(GribData&& other):
        grib_data_(std::move(other.grib_data_)),
        by_common_data_(std::move(other.by_common_data_)),
        by_date_(std::move(other.by_date_)),
        by_grid_(std::move(other.by_grid_)){}
    };
    GribData grib_;
    std::set<Data::FORMAT> unsaved_;
    std::unordered_set<fs::path> files_;
    fs::path data_directory_;

    template<Data::FORMAT>
    void __read__(const fs::path& filename);
    template<Data::FORMAT>
    void __write__(const fs::path& filename){
        assert(false);
    }

    template <size_t I=0>
    void __write_all__(){
        if constexpr (I < data_types.size()) {
            if(unsaved_.contains((FORMAT)(I+1))){
                __write__<data_types[I]>(data_directory_);
                unsaved_.erase((FORMAT)(I+1));
            }
            __write_all__<I + 1>();
        }
    }
    public:
        void save(){
            __write_all__<0>();
        }
        Data():data_directory_(fs::path(get_current_dir_name())/"data"/"bin"){}
        Data(const fs::path& data_dir):data_directory_(data_dir){}
        Data(Data&& other):
        grib_(std::move(other.grib_)),
        unsaved_(std::move(other.unsaved_)),
        files_(std::move(other.files_)),
        data_directory_(std::move(other.data_directory_)){}
        ~Data(){
            save();
        }
        void read(const fs::path& filename);
        bool write(const fs::path& filename);
        template<Data::TYPE,Data::FORMAT>
        bool write( std::vector<char>& buf,Organization center,
                    std::optional<TimeFrame> time_fcst,
                    const std::unordered_set<SearchParamTableVersion>& parameters,
                    TimeInterval time_interval,
                    RepresentationType rep_t,
                    Coord pos);
        void add_data(GribDataInfo& grib_data);
        void add_data(SublimedGribDataInfo& grib_data);
        void add_data(SublimedGribDataInfo&& grib_data);
        bool unsaved() const{
            return !unsaved_.empty();
        }
        const std::unordered_set<path::Storage<false>> paths() const{
            return grib_.grib_data_.paths();
        }
        auto data() const{
            return grib_.grib_data_.data();
        }
        const std::unordered_set<fs::path>& written_files() const{
            return files_;
        }
        const SublimedGribDataInfo& sublimed_data() const{
            return grib_.grib_data_;
        }
        std::unordered_map<path::Storage<true>,SublimedDataInfo> match(
            Organization center,
            uint8_t table_version,
            TimeInterval time_interval,
            RepresentationType rep_t,
            Coord pos
        ) const;
        std::unordered_map<path::Storage<true>,SublimedDataInfo> match(
            Organization center,
            std::optional<TimeFrame> time_fcst,
            const std::unordered_set<SearchParamTableVersion>& parameters,
            TimeInterval time_interval,
            RepresentationType rep_t,
            Coord pos
        ) const;
};