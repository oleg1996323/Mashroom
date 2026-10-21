#pragma once
#include <cstdlib>
#include <cstdint>
#include <vector>
#include "Line.h"
#include "OsterLib/types/coord.h"
#include <future>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <iostream>
#include "algorithms.h"
#include <limits>
#include <span>
#include <ranges>
#include <future>
#include <algorithm>
#include <numeric>
#include <functional>
#include <thread>
#include "Intersection.h"
#include "BasicGeometry.h"

template<typename POINT_T,uint64_t DIMENSIONS = 2>
class BasicPolylineView:
    public BasicGeometry<POINT_T,true,DIMENSIONS>
{
    protected:
    std::vector<const POINT_T> vertices_;
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

// class PolylineView:public geometry::BasicPolylineView<Coord>{
//     public:
//     template<template <typename T> typename RANGE,typename T>
//     PolylineView(const RANGE<T>& vertices):
//         BasicPolylineView(vertices){}

//     std::vector<Intersection> line_intersections(const Line& cut_line) const noexcept{
//         std::vector<Intersection> result;
//         if(cut_line.first()==cut_line.second() || vertices_.empty())
//             return {};
//         for(uint64_t i = 0;i<vertices_.size();++i){
//             auto& first = vertices_[i];
//             auto& second = vertices_[(i+1)%vertices_.size()];
//             if(::intersects(first,second,
//                 cut_line.first(),cut_line.second())){
//                 if(auto found = cut_line.intersection(first.lon_,first.lat_,second.lon_,second.lat_);
//                     found.has_value()){
//                     Coord coord{  .lat_ = found.value().second,
//                             .lon_ = found.value().first};
//                     if(!result.empty()){
//                         if(result.back().point()!=coord)
//                             result.push_back(Intersection(coord,i,0));
//                         else continue;
//                     }
//                     else result.push_back(Intersection(coord,i,0));
//                 }
//                 else continue;
//             }
//             else continue;
//         }
//         std::sort(result.begin(),result.end(),
//             [](const Intersection& lhs,const Intersection& rhs)
//         {
//             return lhs.first_elem_id()<rhs.first_elem_id();
//         });
//         return result;
//     }
// };