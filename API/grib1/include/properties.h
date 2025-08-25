#pragma once
#include "serialization.h"
#include "code_tables/table_0.h"
#include "code_tables/table_4.h"
#include "code_tables/table_6.h"
#include <unordered_set>
#include "types/coord.h"
#include "paramtablev.h"
using namespace std::chrono_literals;
using namespace std::chrono;
#include <types/time_interval.h>
struct SearchProperties{
    std::unordered_set<SearchParamTableVersion> parameters_;
    std::optional<TimeFrame> fcst_unit_;
    std::optional<Organization> center_;
    utc_tp from_date_ = utc_tp(sys_days(1970y/1/1));
    utc_tp to_date_ = system_clock::now();
    std::optional<RepresentationType> grid_type_;
    std::optional<Coord> position_;
};

namespace serialization{
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,SearchParamTableVersion>{
        auto operator()(const SearchParamTableVersion& val,std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.param_,val.t_ver_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SearchParamTableVersion>{
        auto operator()(SearchParamTableVersion& val,std::span<const char> buf) const noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.param_,val.t_ver_);
        }
    };

    template<>
    struct Serial_size<SearchParamTableVersion>{
        auto operator()(const SearchParamTableVersion& val) const noexcept{
            return serial_size(val.param_,val.t_ver_);
        }
    };

    template<>
    struct Min_serial_size<SearchParamTableVersion>{
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(SearchParamTableVersion::param_),decltype(SearchParamTableVersion::t_ver_)>();
        }();
    };

    template<>
    struct Max_serial_size<SearchParamTableVersion>{
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(SearchParamTableVersion::param_),decltype(SearchParamTableVersion::t_ver_)>();
        }();
    };
}