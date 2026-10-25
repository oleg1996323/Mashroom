#pragma once
#include "OsterLib/types/coord.h"
#include <cstdint>
#include <array>
#include "Point.h"

namespace geometry{
    template<uint64_t DIMENSIONS=2>
    class Intersection{
        Point<DIMENSIONS> point_;
        uint64_t id_1_;
        uint64_t id_2_;
        public:
        Intersection(Point<DIMENSIONS> point,uint64_t id_1,uint64_t id_2):
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
        const Point<DIMENSIONS>& point() const noexcept{
            return point_;
        }
        uint64_t first_elem_id() const noexcept{
            return id_1_;
        }
        uint64_t second_elem_id() const noexcept{
            return id_2_;
        }
    };

    template<uint64_t DIMENSIONS>
    void print(const Intersection<DIMENSIONS>& is){
        std::cout<<"intersection: point";print(is.point());
        std::cout<<"first ID: "<<is.first_elem_id()<<"; "<<"second ID: "<<is.second_elem_id()<<std::endl;
    }

    template<uint64_t DIMENSIONS>
    void print(const std::vector<Intersection<DIMENSIONS>>& iss){
        for(const Intersection<DIMENSIONS>& is:iss)
            print(is);
    }
}