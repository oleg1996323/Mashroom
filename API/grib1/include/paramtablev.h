#pragma once
#include <cinttypes>
#include <functional>
#include "generated/code_tables/eccodes_tables.h"
#include "code_tables/table_0.h"
#include "serialization.h"

const ParmTable* parameter_table(Organization center, unsigned char table_version, unsigned char param_num);

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
    bool operator<(const SearchParamTableVersion& other) const{
        if (t_ver_ != other.t_ver_)
            return t_ver_ < other.t_ver_;
        return param_ < other.param_;
    }
};
template<>
struct std::hash<SearchParamTableVersion>{
    size_t operator()(const SearchParamTableVersion& val) const{
        auto hash =(static_cast<size_t>(val.param_) << 8) | static_cast<size_t>(val.t_ver_);
        return (static_cast<size_t>(val.param_) << 8) | static_cast<size_t>(val.t_ver_);
    }
};
template<>
struct std::equal_to<SearchParamTableVersion>{
    bool operator()(const SearchParamTableVersion& lhs,const SearchParamTableVersion& rhs) const{
        return lhs==rhs;
    }
};
template<>
struct std::less<SearchParamTableVersion>{
    bool operator()(const SearchParamTableVersion& lhs,const SearchParamTableVersion& rhs) const{
        return lhs<rhs;
    }
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