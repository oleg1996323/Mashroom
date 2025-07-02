#pragma once
#include <cinttypes>
#include <array>
#include <string_view>
#include <string>
#include <ratio>
#include <array>
#include <tuple>
#include <limits>

enum class UnitsDomain{
    UnitLess,
    InformationSize,
    Time,
    Length,
    Mass
};

using Units_t = UnitsDomain;

template<Units_t T>
struct UnitsInfo;

template<>
struct UnitsInfo<Units_t::InformationSize>{
    static constexpr std::array<const char*,5> abbr = {
        "B",
        "KB",
        "MB",
        "GB",
        "TB"
    };

    static constexpr std::array<const char*,5> name = {
        "bytes",
        "kilobytes",
        "megabytes",
        "gigabytes",
        "terabytes"
    };

    enum type:size_t{
        BYTE,
        KILOBYTE,
        MEGABYTE,
        GIGABYTE,
        TERABYTE
    };
};

/**
 * @brief Only for compile use.
 */

template<intmax_t NUM,intmax_t DEN,typename ENUM,ENUM ENUM_VAL>
requires (DEN!=0)
struct Unit;

template<Units_t DOMAIN>
constexpr UnitsDomain domain(typename UnitsInfo<DOMAIN>::type enum_val){
    return DOMAIN;
}

template<intmax_t NUM,intmax_t DEN,typename ENUM, ENUM ENUM_VAL>
requires (DEN!=0)
struct UnitBase{
    using ratio = std::ratio<NUM,DEN>;
    using Derived = Unit<NUM,DEN,ENUM,ENUM_VAL>;
    using info = UnitsInfo<domain(ENUM{})>;

    constexpr static std::string_view name() noexcept{
        return std::string_view(info::name[ENUM_VAL]);
    }
    constexpr static const char* c_name() noexcept{
        return info::name[ENUM_VAL];
    }
    constexpr static std::string_view abbreviation() noexcept{
        return std::string_view(info::abbr[ENUM_VAL]);
    }
    constexpr static const char* c_abbreviation() noexcept{
        return std::string_view(info::abbr[ENUM_VAL]);
    }
};

template<intmax_t NUM,intmax_t DEN,typename ENUM,ENUM ENUM_VAL>
requires (DEN!=0)
struct Unit:UnitBase<NUM,DEN,ENUM_VAL,ENUM,Unit>{
    static_assert(std::is_enum_v<ENUM>);
    double val_ = 0;
    explicit Unit(double val):val_(val){}
    template<intmax_t NUM_OTHER,intmax_t DEN_OTHER,ENUM ENUM_VAL_OTHER>
    requires (ENUM_VAL_OTHER==ENUM_VAL)
    Unit(Unit<NUM_OTHER,DEN_OTHER,ENUM,ENUM_VAL_OTHER> other){
        using OTHER_ratio = typename decltype(other)::ratio;
        val_ = other.val_*OTHER_ratio::num/OTHER_ratio::den*ratio::den/ratio::num;
    }

    template<intmax_t NUM_OTHER,intmax_t DEN_OTHER,ENUM ENUM_VAL_OTHER>
    requires (ENUM_VAL_OTHER==ENUM_VAL)
    Unit operator=(Unit<NUM_OTHER,DEN_OTHER,ENUM,ENUM_VAL_OTHER> other){
        using OTHER_ratio = typename decltype(other)::ratio;
        val_ = other.val_*OTHER_ratio::num/OTHER_ratio::den*ratio::den/ratio::num;
        return *this;
    }
};

namespace units{
template<intmax_t NUM,intmax_t DEN,typename ENUM,ENUM ENUM_VAL>
using InformationSize = Unit<NUM,DEN,ENUM_VAL>;

template<typename T>
concept Information = requires(T unit) {
    { unit.to_bytes() } -> std::convertible_to<double>;
    requires std::is_same_v<
        typename T::info, 
        UnitsInfo<Units_t::InformationSize>
    >;
};

namespace information{
using type = UnitsInfo<Units_t::InformationSize>::type;
using bytes =       InformationSize<1,1,type,type::BYTE>;
using kilobytes =   InformationSize<1024LL,1,type,type::KILOBYTE>;
using megabytes =   InformationSize<1024LL*1024,1,type,type::MEGABYTE>;
using gigabytes =   InformationSize<1024LL*1024*1024,1,type,type::GIGABYTE>;
using terabytes =   InformationSize<1024LL*1024*1024*1024,1,type,type::TERABYTE>;
}

template<Units_t T>
inline typename UnitsInfo<T>::type from_name(std::string_view cmd) noexcept{
    for(int i=0;i<UnitsInfo<T>::name.size();++i){
        if(cmd==UnitsInfo<T>::name[i])
            return static_cast<UnitsInfo<T>::type>(i);
    }
    return static_cast<UnitsInfo<T>::type>(-1);
}

template<Units_t T>
inline typename UnitsInfo<T>::type from_abbr(std::string_view cmd) noexcept{
    for(int i=0;i<UnitsInfo<T>::name.size();++i){
        if(cmd==UnitsInfo<T>::name[i])
            return static_cast<UnitsInfo<T>::type>(i);
    }
    return static_cast<UnitsInfo<T>::type>(-1);
}

template<typename T>
inline std::string_view to_name(T unit) noexcept{
    if constexpr (UnitsInfo<T>::name.size()>unit)
        return UnitsInfo<T>::name[unit];
    else return ""sv;
}

template<typename T>
inline std::string_view to_abbr(T unit) noexcept{
    if constexpr (UnitsInfo<T>::abbr.size()>unit)
        return UnitsInfo<T>::abbr[unit];
    else return ""sv;
}
}