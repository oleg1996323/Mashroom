#pragma once
#include <cinttypes>
#include <array>
#include <string_view>
#include <string>
#include <ratio>
#include <array>

enum class UnitsDomain{
    InformationSize
};

using Units_t = UnitsDomain;

template<Units_t T>
struct UnitsInfo;

template<>
struct UnitsInfo<Units_t::InformationSize>{
    enum type:uint8_t{
        BYTE,
        KILOBYTE,
        MEGABYTE,
        GIGABYTE,
        TERABYTE
    };
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
};

template<Units_t TYPE,auto UNIT_ENUM,intmax_t NUM,intmax_t DEN=1>
requires std::is_same_v<typename UnitsInfo<TYPE>::type,decltype(UNIT_ENUM)>
struct Unit final{
    using ratio = std::ratio<NUM,DEN>;
    using info = UnitsInfo<TYPE>;
    static_assert(std::is_same_v<decltype(UNIT_ENUM),info::type>);
    static_assert(std::is_enum_v<UNIT_ENUM>);
    double val_;
    explicit Unit(double val):val_(val){}
    template<Units_t TYPE_OTHER,auto ENUM_OTHER,intmax_t NUM_OTHER,intmax_t DEN_OTHER>
    Unit(Unit<TYPE_OTHER,ENUM_OTHER,NUM_OTHER,DEN_OTHER> other){
        using OTHER_ratio = typename decltype(other)::ratio;
        static_assert(TYPE==TYPE_OTHER,"Unable convert different unit domains.");
        if constexpr (std::is_same_v<decltype(UNIT_ENUM), decltype(ENUM_OTHER)>) {
            val_ = other.val_*OTHER_ratio::num/OTHER_ratio::den*ratio::den/ratio::num;
        } else {
            static_assert(false, "Incompatible unit types");
        }
    }

    template<Units_t TYPE_OTHER,auto ENUM_OTHER,intmax_t NUM_OTHER,intmax_t DEN_OTHER>
    Unit& operator=(Unit<TYPE_OTHER,ENUM_OTHER,NUM_OTHER,DEN_OTHER> other){
        using OTHER_ratio = typename decltype(other)::ratio;
        static_assert(TYPE==TYPE_OTHER,"Unable convert different unit domains.");
        if constexpr (std::is_same_v<decltype(UNIT_ENUM), decltype(ENUM_OTHER)>) {
            val_ = other.val_*OTHER_ratio::num/OTHER_ratio::den*ratio::den/ratio::num;
        } else {
            static_assert(false, "Incompatible unit types");
        }
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
        else return ""
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
