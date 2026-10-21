#pragma once
#include <vector>
#include "Intersection.h"

namespace geometry{
    template<typename G1,typename G2>
    std::vector<Intersection<G1::value_t>> intersections(const G1& geom1,const G2& geom2) noexcept;

    template<typename G1, typename G2>
    struct GetIntersections
    {
        std::vector<Intersection<G::value_t>> operator()(const G1& geom1,const G2& geom2) 
            const noexcept(noexcept(intersections(geom1,geom2)))
        {
            return intersections(geom1,geom2);
        }
    };
}