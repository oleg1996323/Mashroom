#pragma once
#ifdef GRIB1API
#include "OsterLib/serialization.h"
#include "grib1/code_tables.h"
#include <unordered_set>
#include <optional>
#include "OsterLib/types/coord.h"
#include "grib1/paramtableversion.h"
#include "grib1/code_tables.h"
#include "grib1/sections.h"
using namespace std::chrono_literals;
using namespace std::chrono;
#include "OsterLib/types/time_interval.h"
struct SearchProperties{
    std::unordered_set<SearchParamTableVersion> parameters_;
    std::optional<utc_tp_t<std::chrono::seconds>> from_date_;
    std::optional<utc_tp_t<std::chrono::seconds>> to_date_;
    std::optional<DateTimeDiff> diff_;
    std::optional<Coord> position_;
    std::optional<Organization> center_;
    std::optional<TimeForecast> fcst_unit_;
    std::optional<Level> level_;
    std::optional<RepresentationType> grid_type_;

    #ifdef DEBUG
        bool operator==(const SearchProperties& other) const noexcept{
            return parameters_==other.parameters_ &&
            from_date_==other.from_date_ &&
            to_date_==other.to_date_ &&
            diff_==other.diff_ &&
            position_==other.position_ &&
            center_==other.center_ &&
            fcst_unit_==other.fcst_unit_ &&
            level_ == other.level_ &&
            grid_type_==other.grid_type_;
        }
    #endif
};

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,SearchProperties>{
        auto operator()(const SearchProperties& val,
                        std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,
                                val.parameters_,
                                val.fcst_unit_,
                                val.center_,
                                val.from_date_,
                                val.to_date_,
                                val.diff_,
                                val.grid_type_,
                                val.position_,
                                val.level_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SearchProperties>{
        auto operator()(SearchProperties& val,
                        StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,
                                val.parameters_,
                                val.fcst_unit_,
                                val.center_,
                                val.from_date_,
                                val.to_date_,
                                val.diff_,
                                val.grid_type_,
                                val.position_,
                                val.level_);
        }
    };

    template<>
    struct Serial_size<SearchProperties>{
        auto operator()(const SearchProperties& val) const noexcept{
            return serial_size(val.parameters_,
                                val.fcst_unit_,
                                val.center_,
                                val.from_date_,
                                val.to_date_,
                                val.diff_,
                                val.grid_type_,
                                val.position_,
                                val.level_);
        }
    };

    template<>
    struct Min_serial_size<SearchProperties>{
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(SearchProperties::parameters_),
            decltype(SearchProperties::fcst_unit_)
            ,decltype(SearchProperties::center_),
            decltype(SearchProperties::from_date_),
            decltype(SearchProperties::to_date_),
            decltype(SearchProperties::diff_),
            decltype(SearchProperties::grid_type_),
            decltype(SearchProperties::position_),
            decltype(SearchProperties::level_)>();
        }();
    };

    template<>
    struct Max_serial_size<SearchProperties>{
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(SearchProperties::parameters_),
            decltype(SearchProperties::fcst_unit_),
            decltype(SearchProperties::center_),
            decltype(SearchProperties::from_date_),
            decltype(SearchProperties::to_date_),
            decltype(SearchProperties::grid_type_),
            decltype(SearchProperties::position_),
            decltype(SearchProperties::level_)>();
        }();
    };
}
#endif