#pragma once
#include "serialization.h"
#include "code_tables/table_0.h"
#include "code_tables/table_4.h"
#include "code_tables/table_6.h"
#include <unordered_set>
#include <optional>
#include "types/coord.h"
#include "paramtablev.h"
#include "code_tables.h"
#include "API/grib1/include/sections/product/levels.h"
#include "API/grib1/include/sections/product/time_forecast.h"
using namespace std::chrono_literals;
using namespace std::chrono;
#include "types/time_interval.h"
struct SearchProperties{
    std::unordered_set<SearchParamTableVersion> parameters_;
    utc_tp from_date_ = utc_tp(sys_days(1970y/1/1));
    utc_tp to_date_ = system_clock::now();
    std::optional<Coord> position_;
    std::optional<RepresentationType> grid_type_;
    std::optional<Organization> center_;
    std::optional<TimeForecast> fcst_unit_;
    std::optional<Level> level_;
};

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,SearchProperties>{
        auto operator()(const SearchProperties& val,std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.parameters_,val.fcst_unit_,val.center_,val.from_date_,val.to_date_,val.grid_type_,val.position_,val.level_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SearchProperties>{
        auto operator()(SearchProperties& val,std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.parameters_,val.fcst_unit_,val.center_,val.from_date_,val.to_date_,val.grid_type_,val.position_,val.level_);
        }
    };

    template<>
    struct Serial_size<SearchProperties>{
        auto operator()(const SearchProperties& val) const noexcept{
            return serial_size(val.parameters_,val.fcst_unit_,val.center_,val.from_date_,val.to_date_,val.grid_type_,val.position_,val.level_);
        }
    };

    template<>
    struct Min_serial_size<SearchProperties>{
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(SearchProperties::parameters_),decltype(SearchProperties::fcst_unit_)
            ,decltype(SearchProperties::center_),decltype(SearchProperties::from_date_),decltype(SearchProperties::to_date_),
            decltype(SearchProperties::grid_type_),decltype(SearchProperties::position_),
            decltype(SearchProperties::level_)>();
        }();
    };

    template<>
    struct Max_serial_size<SearchProperties>{
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(SearchProperties::parameters_),decltype(SearchProperties::fcst_unit_)
            ,decltype(SearchProperties::center_),decltype(SearchProperties::from_date_),decltype(SearchProperties::to_date_),
            decltype(SearchProperties::grid_type_),decltype(SearchProperties::position_),
            decltype(SearchProperties::level_)>();
        }();
    };
}