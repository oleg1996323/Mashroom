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
    InformationSize,
    Time,
    Length,
    Mass,
    Derivative
};

consteval size_t consthash(std::string_view sv){
    size_t hash = std::numeric_limits<uint64_t>::max();
    for (char c : sv) {
        hash ^= static_cast<uint64_t>(c);
        hash *= std::numeric_limits<uint32_t>::max();
    }
    return hash;
}

using Units_t = UnitsDomain;

template<Units_t DOM,int COUNT>
struct UnitDomainCount{
    constexpr static Units_t domain = DOM;
    constexpr static int count = COUNT;
};

template<Units_t DOM,int COUNT1,int COUNT2>
constexpr UnitDomainCount<DOM,COUNT1-COUNT2> operator-(UnitDomainCount<DOM,COUNT1>,UnitDomainCount<DOM,COUNT2>){
    return UnitDomainCount<DOM,COUNT1-COUNT2>();
}

template<Units_t DOM,int COUNT1,int COUNT2>
constexpr UnitDomainCount<DOM,COUNT1+COUNT2> operator+(UnitDomainCount<DOM,COUNT1>,UnitDomainCount<DOM,COUNT2>){
    return UnitDomainCount<DOM,COUNT1+COUNT2>();
}

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

template<template <auto> typename UNITS_INFO,Units_t DOMAIN>
consteval bool is_correct_enum(typename UNITS_INFO<DOMAIN>::type enum_val){
    return true;
}

template<template <auto> typename UNITS_INFO,Units_t DOMAIN>
consteval bool is_derivated(typename UNITS_INFO<DOMAIN>::type enum_val){
    if constexpr (DOMAIN==Units_t::Derivative)
        return true;
    else return false;
}

//todo make tuple checking
template<template <auto> typename UNITS_INFO,Units_t DOMAIN>
consteval bool is_pure(typename UNITS_INFO<DOMAIN>::type enum_val){
    if constexpr (DOMAIN==Units_t::Derivative)
        return true;
    else return false;
}

template<template<typename...> typename TUPLE1,template<typename...> typename TUPLE2,typename... UNIT_DOM_COUNT1,typename... UNIT_DOM_COUNT2>
consteval auto operator/(const TUPLE1<UNIT_DOM_COUNT1...>& tuple1,const TUPLE2<UNIT_DOM_COUNT2...>& tuple2){
    return std::tuple<UNIT_DOM_COUNT1-UNIT_DOM_COUNT2,...>;
}

template<intmax_t NUM,intmax_t DEN,auto UNIT_ENUM>
requires (is_correct_enum(UNIT_ENUM) && !is_derivated(UNIT_ENUM) && DEN!=0)
struct Unit final{
    using ratio = std::ratio<NUM,DEN>;
    using info = UnitsInfo<TYPE>;

    static_assert(std::is_same_v<decltype(UNIT_ENUM),info::type>);
    static_assert(std::is_enum_v<UNIT_ENUM>);
    double val_ = 0;
    explicit Unit(double val):val_(val){}
    template<intmax_t NUM_OTHER,intmax_t DEN_OTHER,auto ENUM_OTHER>
    requires (ENUM_OTHER==UNIT_ENUM)
    Unit(Unit<NUM_OTHER,DEN_OTHER,TYPE,ENUM_OTHER> other){
        using OTHER_ratio = typename decltype(other)::ratio;
        val_ = other.val_*OTHER_ratio::num/OTHER_ratio::den*ratio::den/ratio::num;
    }

    template<intmax_t NUM_OTHER,intmax_t DEN_OTHER,auto ENUM_OTHER>
    requires (ENUM_OTHER==UNIT_ENUM)
    Unit operator=(Unit<NUM_OTHER,DEN_OTHER,TYPE,ENUM_OTHER> other){
        using OTHER_ratio = typename decltype(other)::ratio;
        val_ = other.val_*OTHER_ratio::num/OTHER_ratio::den*ratio::den/ratio::num;
        return *this;
    }

    template<intmax_t NUM_OTHER,intmax_t DEN_OTHER,auto ENUM_OTHER>
    std::conditional_t<ENUM_OTHER==UNIT_ENUM,
    Unit<NUM_OTHER/NUM,DEN/DEN_OTHER,
     operator/(Unit<NUM_OTHER,DEN_OTHER,TYPE,ENUM_OTHER> other){
        using OTHER_ratio = typename decltype(other)::ratio;
        val_ = other.val_*OTHER_ratio::num/OTHER_ratio::den*ratio::den/ratio::num;
        return *this;
    }

    constexpr static std::string_view name() noexcept{
        if constexpr(UNIT_ENUM<info::name.size())
            return std::string_view(info::name[UNIT_ENUM]);
        else return std::string_view();
    }
    constexpr static const char* c_name() noexcept{
        if constexpr(UNIT_ENUM<info::name.size())
            return info::name[UNIT_ENUM];
        else return "";
    }
    constexpr static std::string_view abbreviation() noexcept{
        if constexpr(UNIT_ENUM<info::abbr.size())
            return std::string_view(info::abbr[UNIT_ENUM]);
        else return std::string_view();
    }
    constexpr static const char* c_abbreviation() noexcept{
        if constexpr(UNIT_ENUM<info::abbr.size())
            return info::abbr[UNIT_ENUM];
        else return ""std::get_time()
    }
};

namespace units{
template<auto UNIT_ENUM,intmax_t NUM,intmax_t DEN=1>
using InformationSize = Unit<Units_t::InformationSize,UNIT_ENUM,NUM,DEN>;

template<typename T>
concept Information = requires(T unit) {
    { unit.to_bytes() } -> std::convertible_to<double>;
    requires std::is_same_v<
        typename T::info, 
        UnitsInfo<Units_t::InformationSize>
    >;
};

namespace information{
using bytes =       InformationSize<UnitsInfo<Units_t::InformationSize>::BYTE,1>;
using kilobytes =   InformationSize<UnitsInfo<Units_t::InformationSize>::KILOBYTE,1024LL>;
using megabytes =   InformationSize<UnitsInfo<Units_t::InformationSize>::MEGABYTE,1024LL*1024>;
using gigabytes =   InformationSize<UnitsInfo<Units_t::InformationSize>::GIGABYTE,1024LL*1024*1024>;
using terabytes =   InformationSize<UnitsInfo<Units_t::InformationSize>::TERABYTE,1024LL*1024*1024*1024>;
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
    else return "";
}

template<typename T>
inline std::string_view to_abbr(T unit) noexcept{
    if constexpr (UnitsInfo<T>::abbr.size()>unit)
        return UnitsInfo<T>::abbr[unit];
    else return "";
}
}
