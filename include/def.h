#pragma once
#include <optional>
#include <chrono>
#include <unordered_set>
#include <code_tables/table_6.h>
#include "types/coord.h"
#include "data/common.h"

enum class TimeSeparation{
    HOUR,
    DAY,
    MONTH,
    YEAR
};
struct SearchParamTableVersion{
    uint8_t param_;
    uint8_t t_ver_;
    bool operator==(const SearchParamTableVersion& other) const{
        return param_==other.param_ && t_ver_==other.t_ver_;
    }
};
template<>
struct std::hash<SearchParamTableVersion>{
    size_t operator()(const SearchParamTableVersion& val) const{
        return std::hash<uint8_t>{}(val.param_<<8)+std::hash<uint8_t>{}(val.t_ver_);
    }
    size_t operator()(SearchParamTableVersion&& val) const{
        return std::hash<uint8_t>{}(val.param_<<8)+std::hash<uint8_t>{}(val.t_ver_);
    }
};
template<>
struct std::equal_to<SearchParamTableVersion>{
    bool operator()(const SearchParamTableVersion& lhs,const SearchParamTableVersion& rhs) const{
        return lhs.param_==rhs.param_ && lhs.t_ver_==rhs.t_ver_;
    }
};
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
        auto operator()(const SearchParamTableVersion& val,std::vector<char>& buf) noexcept{
            return serialize<NETWORK_ORDER>(val,buf,val.param_,val.t_ver_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,SearchParamTableVersion>{
        auto operator()(SearchParamTableVersion& val,std::span<const char> buf) noexcept{
            return deserialize<NETWORK_ORDER>(val,buf,val.param_,val.t_ver_);
        }
    };

    template<>
    struct Serial_size<SearchParamTableVersion>{
        auto operator()(const SearchParamTableVersion& val) noexcept{
            return serial_size(val.param_,val.t_ver_);
        }
    };

    template<>
    struct Min_serial_size<SearchParamTableVersion>{
        constexpr auto operator()(const SearchParamTableVersion& val) noexcept{
            return min_serial_size(val.param_,val.t_ver_);
        }
    };

    template<>
    struct Max_serial_size<SearchParamTableVersion>{
        constexpr auto operator()(const SearchParamTableVersion& val) noexcept{
            return max_serial_size(val.param_,val.t_ver_);
        }
    };
}