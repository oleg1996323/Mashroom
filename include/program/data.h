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

using namespace std::chrono;
using namespace std::string_literals;

namespace fs = std::filesystem;
class Data:public __Data__{
    public:
        using sublimed_data_by_common_data = std::unordered_map<std::weak_ptr<CommonDataProperties>,std::unordered_set<path::Storage<true>>>;
        using sublimed_data_by_date_time = std::map<TimeInterval,std::unordered_set<path::Storage<true>>>;
        using sublimed_data_by_grid = std::unordered_map<std::optional<GridInfo>,std::unordered_set<path::Storage<true>>>;
        enum class TYPE:uint8_t{
            METEO,
            TOPO,
            KADASTR
        };
        enum class ACCESS:uint8_t{
            PUBLIC,
            PROTECTED,
            PRIVATE
        };
    
    private:
    template<TYPE DATA_T,FORMAT DATA_F>
    struct __Data_type__{
        static constexpr FORMAT format_type() noexcept{
            return DATA_F;
        }
        static constexpr TYPE data_type() noexcept{
            return DATA_T;
        }
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
        const std::unordered_set<path::Storage<true>>& paths() const{
            return grib_.grib_data_.paths();
        }
        auto data() const{
            return grib_.grib_data_.data();
        }
        const std::unordered_map<__Data__::FORMAT,fs::path>& written_files() const{
            return files_;
        }
        const SublimedGribDataInfo& sublimed_data() const{
            return grib_.grib_data_;
        }

        int find_all(std::optional<RepresentationType> grid_type_,
                        std::optional<TimeInterval> time_,
                        std::optional<TimeFrame> forecast_preference_,
                        utc_tp last_update_) const{
            std::optional<GridInfo> grid_data_;
            std::vector<ptrdiff_t> buf_pos_;
            utc_tp from_ = utc_tp::max();
            utc_tp to_ = utc_tp::min();
            std::chrono::system_clock::duration discret_ = std::chrono::system_clock::duration(0);

        }

        std::unordered_map<path::Storage<true>,SublimedDataInfo> match_data(
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
};