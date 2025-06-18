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
    template<>
    size_t serial_size(const SearchParamTableVersion& sptv) noexcept{
        return serial_size(sptv.param_,sptv.t_ver_);
    }

    template<>
    constexpr size_t min_serial_size(const SearchParamTableVersion& sptv) noexcept{
        return min_serial_size(sptv.param_,sptv.t_ver_);
    }

    template<>
    constexpr size_t max_serial_size(const SearchParamTableVersion& sptv) noexcept{
        return max_serial_size(sptv.param_,sptv.t_ver_);
    }

    template<bool NETWORK_ORDER>
    SerializationEC serialize(const SearchParamTableVersion& sptv, std::vector<char>& buf) noexcept{
        return serialize<NETWORK_ORDER>(sptv,buf,sptv.param_,sptv.t_ver_);
    }

    template<>
    SerializationEC deserialize<true>(SearchParamTableVersion& sptv, std::span<const char> buf) noexcept{
        return deserialize<true>(sptv,buf,sptv.param_,sptv.t_ver_);
    }
}