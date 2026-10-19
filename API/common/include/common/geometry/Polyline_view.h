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
#include "intersection.h"
#include "BasicGeometry.h"

class PolylineView:public geometry::BasicPolylineView<Coord>{
    public:
    template<template <typename T> typename RANGE,typename T>
    PolylineView(const RANGE<T>& vertices):
        BasicPolylineView(vertices){}

    std::vector<Intersection> line_intersections(const Line& cut_line) const noexcept{
        std::vector<Intersection> result;
        if(cut_line.first()==cut_line.second() || vertices_.empty())
            return {};
        for(uint64_t i = 0;i<vertices_.size();++i){
            auto& first = vertices_[i];
            auto& second = vertices_[(i+1)%vertices_.size()];
            if(::intersects(first,second,
                cut_line.first(),cut_line.second())){
                if(auto found = cut_line.intersection(first.lon_,first.lat_,second.lon_,second.lat_);
                    found.has_value()){
                    Coord coord{  .lat_ = found.value().second,
                            .lon_ = found.value().first};
                    if(!result.empty()){
                        if(result.back().point()!=coord)
                            result.push_back(Intersection(coord,i,0));
                        else continue;
                    }
                    else result.push_back(Intersection(coord,i,0));
                }
                else continue;
            }
            else continue;
        }
        std::sort(result.begin(),result.end(),
            [](const Intersection& lhs,const Intersection& rhs)
        {
            return lhs.first_elem_id()<rhs.first_elem_id();
        });
        return result;
    }
};