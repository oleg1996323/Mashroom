#pragma once
#include <OsterLib/types/coord.h>
#include "BasicGeometry.h"
#include "Point.h"

namespace geometry{
    template<uint64_t DIMENSIONS1,typename VALUE1_T,
        uint64_t DIMENSIONS2,typename VALUE2_T,
        uint64_t DIMENSIONS3,typename VALUE3_T>
    long double cross(const BasicPoint<DIMENSIONS1,VALUE1_T>& a,
            const BasicPoint<DIMENSIONS2,VALUE2_T>& b, 
            const BasicPoint<DIMENSIONS3,VALUE3_T>& c);
    bool on_segment(const Coord& a, const Coord& b, const Coord& c);
    bool intersects(const Coord& first_1, const Coord& second_1, const Coord& first_2,const Coord& second_2) noexcept;
    long double pythagoras(const Coord& first,const Coord& second) noexcept;
    long double pythagoras2(const Coord& first,const Coord& second) noexcept;

    template<typename VALUE1_T,
        typename VALUE2_T,
        typename VALUE3_T>
    long double cross(const Point<2,VALUE1_T>& a,
            const Point<2,VALUE2_T>& b, 
            const Point<2,VALUE3_T>& c) {
        using value_result_t = std::common_type_t<
            std::common_type_t<VALUE1_T,VALUE2_T>,VALUE3_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        return (static_cast<max_prec_t>(exec<Get,0>(b)) - 
                static_cast<max_prec_t>(exec<Get,0>(a))) * 
                (static_cast<max_prec_t>(exec<Get,1>(c)) - 
                static_cast<max_prec_t>(exec<Get,1>(a)))
            - (static_cast<max_prec_t>(exec<Get,1>(b)) - 
                static_cast<max_prec_t>(exec<Get,1>(a))) * 
                (static_cast<max_prec_t>(exec<Get,0>(c)) - 
                static_cast<max_prec_t>(exec<Get,0>(a)));
    }

    template<typename VALUE1_T,
        typename VALUE2_T,
        typename VALUE3_T>
    bool on_segment(const Point<2,VALUE1_T>& a, const Point<2,VALUE2_T>& b, const Point<2,VALUE3_T>& c) 
    {
        using value_result_t = std::common_type_t<VALUE1_T,VALUE2_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        return std::min(exec<Get,0>(a), exec<Get,0>(b)) <= exec<Get,0>(c) && 
                exec<Get,0>(c) <= std::max(exec<Get,0>(a), exec<Get,0>(b))
            && std::min(exec<Get,1>(a), exec<Get,1>(b)) <= exec<Get,1>(c) && 
                exec<Get,1>(c) <= std::max(exec<Get,1>(a), exec<Get,1>(b));
    }

    template<typename VALUE1_T,
        typename VALUE2_T>
    long double pythagoras2(
            const Point<2,VALUE1_T>& first,
            const Point<2,VALUE2_T>& second) noexcept{
        long double dY = exec<Get,1>(first)-exec<Get,1>(second);
        long double dX = exec<Get,0>(first)-exec<Get,0>(second);
        return dY*dY+dX*dX;
    }
    template<typename VALUE1_T,
        typename VALUE2_T>
    long double pythagoras(
            const Point<2,VALUE1_T>& first,
            const Point<2,VALUE2_T>& second) noexcept{
        return std::sqrtl(pythagoras2(first,second));
    }
}