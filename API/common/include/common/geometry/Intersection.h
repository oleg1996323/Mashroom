#pragma once
#include "OsterLib/types/coord.h"
#include <cstdint>
#include <array>

template<typename POINT_T,uint64_t DIMENSIONS = dimensions<POINT_T>()>
class BasicPoint;

template<uint64_t DIMENSIONS,typename VALUE_T = long double>
using Point = BasicPoint<std::array<VALUE_T,DIMENSIONS>,DIMENSIONS>;

template<uint64_t DIMENSIONS=2>
class Intersection{
    Point<DIMENSIONS> point_;
    uint64_t id_1_;
    uint64_t id_2_;
    public:
    Intersection(POINT_T point,uint64_t id_1,uint64_t id_2):
        point_(point),id_1_(id_1),id_2_(id_2){}
    Intersection(const Intersection& other) noexcept:
        point_(other.point_),
        id_1_(other.id_1_),
        id_2_(other.id_2_){}
    Intersection(Intersection&& other) noexcept:
        point_(other.point_),
        id_1_(other.id_1_),
        id_2_(other.id_2_){}
    Intersection& operator=(const Intersection& other) noexcept{
        if(this!=&other){
            point_ = other.point_;
            id_1_ = other.id_1_;
            id_2_ = other.id_2_;
        }
        return *this;
    }
    Intersection& operator=(Intersection&& other) noexcept{
        if(this!=&other){
            point_ = other.point_;
            id_1_ = other.id_1_;
            id_2_ = other.id_2_;
        }
        return *this;
    }
    const std::array<long double,DIMENSIONS>& point() const noexcept{
        return point_;
    }
    uint64_t first_elem_id() const noexcept{
        return id_1_;
    }
    uint64_t second_elem_id() const noexcept{
        return id_2_;
    }
};