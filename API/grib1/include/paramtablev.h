#pragma once
#include <cinttypes>
#include <functional>
#include "generated/code_tables/eccodes_tables.h"
#include "code_tables/table_0.h"

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