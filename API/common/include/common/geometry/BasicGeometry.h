#pragma once
#include <type_traits>
#include <utility>
#include <cstdint>
#include <cmath>
#include <vector>
#include <span>
#include "OsterLib/types/coord.h"
#include "intersection.h"
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

template<uint64_t DIMENSION,typename POINT_T>
struct Get{
    template<typename VALUE_T>
    VALUE_T operator()() const = delete;
};

template<typename G1,typename G2>
std::vector<Intersection<typename G1::value_t>> intersections(const G1& geom1,const G2& geom2) noexcept;
template<typename G_CUTTED,typename G_CUTTING>
std::vector<G_CUTTED> cut(
    const G_CUTTED& cutted,
    const G_CUTTING& cutting) noexcept;
template<typename G1,typename G2>
bool intersects(const G1& geom1,const G2& geom2) noexcept;

template<typename G1, typename G2>
struct GetIntersections
{
    std::vector<Intersection<G::value_t>> operator()(const G1& geom1,const G2& geom2) 
        const noexcept(noexcept(intersections(geom1,geom2)))
    {
        return intersections(geom1,geom2);
    }
};

template<typename G1, typename G2>
struct GetIntersects
{
    bool operator()(const G1& geom1,const G2& geom2) 
        const noexcept(noexcept(intersects(geom1,geom2)))
    {
        return intersects(geom1,geom2);
    }
};

template<typename G1, typename G2>
struct GetCut
{
    std::vector<G1> operator()(const G1& geom1,const G2& geom2) 
        const noexcept(noexcept(cut(geom1,geom2)))
    {
        return cut(geom1,geom2);
    }
};

template<uint64_t,typename,typename = void>
struct defined_get_coordinate:std::false_type{};

template<uint64_t DIMENSION,typename POINT_T>
struct defined_get_coordinate<DIMENSION,POINT_T,
std::void_t<typename std::invoke_result<Get<DIMENSION,POINT_T>,POINT_T>::type>>:std::true_type{};

template<bool VIEW,uint64_t DIMENSIONS>
class BasicGeometry{
    static_assert(DIMENSIONS>0,"Not all Get::operator() are defined");
    static_assert(
    []<std::size_t... Is>(std::index_sequence<Is...>) {
        return (defined_get_coordinate<Is, POINT_T>::value && ...);
    }(std::make_index_sequence<DIMENSIONS>{}),
    "Point requires Get<i, POINT_T> for i in [0, DIMENSIONS)");
    static_assert(
    []<std::size_t... Is>(std::index_sequence<Is...>) {
        (ValueTypeCheck<typename std::invoke_result<Get<Is,POINT_T>,POINT_T>::type>(), ...);
        return true;
    }(std::make_index_sequence<DIMENSIONS>{}),
    "Point requires Get<i, POINT_T> for i in [0, DIMENSIONS)");
    public:
    template<uint64_t DIMENSION>
    using value_t = typename std::invoke_result<Get<DIMENSION,POINT_T>,POINT_T>::type;

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

template<typename USER_POINT_T, uint64_t D = 0, uint64_t MAX = MAX_ALLOWABLE_DIMENSIONS>
consteval uint64_t dimensions() noexcept {
    static_assert(D<=MAX,MAX_ALLOWABLE_DIMENSIONS_STR(MAX_ALLOWABLE_DIMENSIONS));
    if constexpr (D >= MAX)
        return MAX;
    else if constexpr (defined_get_coordinate<D, USER_POINT_T>::value)
        return dimensions<USER_POINT_T, D + 1, MAX>();
    else
        return D;
}

template<typename POINT_T,uint64_t DIMENSIONS = 2>
class BasicPolyline:
    public BasicGeometry<POINT_T,false,DIMENSIONS>
{
    protected:
    std::vector<POINT_T> vertices_;
    public:
    template<template <typename T,typename...> typename RANGE,
        std::enable_if_t<std::contiguous_iterator<typename RANGE<POINT_T>::iterator>>>
    BasicPolyline(const RANGE<POINT_T>& vertices)
    {
        std::vector<int> v(vertices.begin(),vertices.end());
    }
    template<template <typename T,typename...> typename RANGE,
        std::enable_if_t<std::contiguous_iterator<typename RANGE<POINT_T>::iterator>>>
    BasicPolyline(RANGE<POINT_T>&& vertices)
    {
        std::vector<int> v(std::make_move_iterator(vertices.begin()),std::make_move_iterator(vertices.end()));
    }
    BasicPolyline(const std::vector<POINT_T>& vertices):
        vertices_(vertices.begin(),vertices.end()){}
    BasicPolyline(std::vector<POINT_T>&& vertices):
        vertices_(std::make_move_iterator(vertices.begin()),std::make_move_iterator(vertices.end())){}
    const std::vector<Coord>& vertices() const noexcept{
        return vertices_;
    }
    size_t number_vertices() const noexcept{
        return vertices_.size();
    }
    size_t number_segments() const noexcept{
        return vertices_.size()<2?0:vertices_.size()-1;
    }
};

template<typename POINT_T,uint64_t DIMENSIONS = 2>
class BasicPolylineView:
    public BasicGeometry<POINT_T,true,DIMENSIONS>
{
    protected:
    std::span<const POINT_T> vertices_;
    public:
    template<template <typename T,typename...> typename RANGE,
        std::enable_if_t<std::contiguous_iterator<typename RANGE<POINT_T>::iterator>>>
    BasicPolylineView(const RANGE<POINT_T>& vertices):
        vertices_(vertices.begin(),vertices.end()){}
    std::span<Coord> vertices() const noexcept{
        return vertices_;
    }
    size_t number_vertices() const noexcept{
        return vertices_.size();
    }
    size_t number_segments() const noexcept{
        return vertices_.size()<2?0:vertices_.size()-1;
    }
};
}