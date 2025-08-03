#pragma once
#include <type_traits>
#include <cinttypes>
#include <concepts>
#include <bit>

namespace detail{

template<int sz, bool signed_t = false>
requires (sz<=8 && sz>0)
struct to_integer_type_impl{
    using type = ::std::conditional_t<
        signed_t,
        std::conditional_t<sz==1,int8_t,
        std::conditional_t<sz==2,int16_t,
        std::conditional_t<sz<=4,int32_t,
        int64_t>>>,
        std::conditional_t<sz==1,uint8_t,
        std::conditional_t<sz==2,uint16_t,
        std::conditional_t<sz<=4,uint32_t,
        uint64_t>>>>;
};
template<int sz>
requires (sz<=8 && sz>0)
struct to_float_type_impl{
    using type = ::std::conditional_t<sz<=4,float,std::conditional_t<sz<=8,double,void>>;
};

template<int sz,bool signed_t = false>
using to_integer_type = to_integer_type_impl<sz,signed_t>::type;
template<int sz>
using to_float_type = to_float_type_impl<sz>::type;
}

template<typename T>
requires std::is_floating_point_v<std::decay_t<T>>
constexpr auto to_integer(T&& fl_p) noexcept{
    using IntType = detail::to_integer_type<sizeof(T)>;
    return std::bit_cast<IntType>(fl_p);
}

template<typename T>
requires std::is_integral_v<std::decay_t<T>>
constexpr auto to_float(T&& integer) noexcept{
    using FloatType = ::detail::to_float_type<sizeof(T)>;
    return std::bit_cast<FloatType>(integer);
}