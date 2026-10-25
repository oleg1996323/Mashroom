#pragma once
#include <type_traits>
#include <utility>
#include <cstdint>
#include <cmath>
#include <vector>
#include <span>
#include "OsterLib/types/coord.h"
#include <format>

#define MAX_ALLOWABLE_DIMENSIONS 4
#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)
#define TAGGED_FUNCTOR(NAME) NAME
#define MAX_ALLOWABLE_DIMENSIONS_STR(MAX) "Overflow maximum allowed dimensions " STRINGIFY(MAX)

namespace geometry{

template<uint64_t UINT1,uint64_t UINT2>
struct Max{
    static constexpr uint64_t value = (UINT1<UINT2)?UINT2:UINT1;
};

template<uint64_t UINT1,uint64_t UINT2>
struct Min{
    static constexpr uint64_t value = (UINT1>UINT2)?UINT2:UINT1;
};

template<typename,typename,typename = void>
struct has_operator_plus:std::false_type{};

template<typename VALUE1,typename VALUE2>
struct has_operator_plus<VALUE1,VALUE2,
    std::void_t<decltype(std::declval<VALUE1>()+std::declval<VALUE2>())>>:std::true_type{};

template<typename,typename,typename = void>
struct has_operator_minus:std::false_type{};

template<typename VALUE1,typename VALUE2>
struct has_operator_minus<VALUE1,VALUE2,
    std::void_t<decltype(std::declval<VALUE1>()-std::declval<VALUE2>())>>:std::true_type{};

template<typename,typename = void>
struct has_operator_uminus:std::false_type{};

template<typename VALUE>
struct has_operator_uminus<VALUE,
    std::void_t<decltype(-std::declval<VALUE>())>>:std::true_type{};

template<typename,typename,typename = void>
struct has_operator_multiplication:std::false_type{};

template<typename VALUE1,typename VALUE2>
struct has_operator_multiplication<VALUE1,VALUE2,
    std::void_t<decltype(std::declval<VALUE1>()*std::declval<VALUE2>())>>:std::true_type{};

template<typename,typename,typename = void>
struct has_operator_division:std::false_type{};

template<typename VALUE1,typename VALUE2>
struct has_operator_division<VALUE1,VALUE2,
    std::void_t<decltype(std::declval<VALUE1>()/std::declval<VALUE2>())>>:std::true_type{};

template<typename VALUE_T>
struct ValueTypeCheck{
    static_assert(has_operator_plus<VALUE_T,VALUE_T>::value);
    static_assert(has_operator_minus<VALUE_T,VALUE_T>::value);
    static_assert(has_operator_uminus<VALUE_T>::value);
    static_assert(has_operator_multiplication<VALUE_T,VALUE_T>::value);
    static_assert(has_operator_division<VALUE_T,VALUE_T>::value);
    static_assert(has_operator_plus<VALUE_T,int>::value);
    static_assert(has_operator_minus<VALUE_T,int>::value);
    static_assert(has_operator_multiplication<VALUE_T,int>::value);
    static_assert(has_operator_division<VALUE_T,int>::value);
    static_assert(has_operator_plus<VALUE_T,unsigned int>::value);
    static_assert(has_operator_minus<VALUE_T,unsigned int>::value);
    static_assert(has_operator_multiplication<VALUE_T,unsigned int>::value);
    static_assert(has_operator_division<VALUE_T,unsigned int>::value);
    static_assert(has_operator_plus<VALUE_T,std::float_t>::value);
    static_assert(has_operator_minus<VALUE_T,std::float_t>::value);
    static_assert(has_operator_multiplication<VALUE_T,std::float_t>::value);
    static_assert(has_operator_division<VALUE_T,std::float_t>::value);
};

template<bool VIEW,uint64_t DIMENSIONS>
class BasicGeometry{
    static_assert(DIMENSIONS>0,"Not all Get::operator() are defined");
    public:
    BasicGeometry() = default;
    BasicGeometry(const BasicGeometry&) = default;
    BasicGeometry(BasicGeometry&&) noexcept = default;
    BasicGeometry& operator=(const BasicGeometry&) = default;
    BasicGeometry& operator=(BasicGeometry&&) noexcept = default;

    static constexpr uint64_t dimensions() noexcept{
        return DIMENSIONS;
    }
    static constexpr bool is_view() noexcept{
        return VIEW;
    }

    template<typename FUNCTOR, typename OTHER_GEOMETRY>
    auto execute(const FUNCTOR& functor, const OTHER_GEOMETRY& other) const
        noexcept(noexcept(functor(*this, other)))
    {
        return functor(*this, other);
    }
};
}