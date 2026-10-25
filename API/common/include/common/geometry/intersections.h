#pragma once
#include <vector>
#include "Intersection.h"

namespace geometry{
    template<typename G1,typename G2>
    std::vector<Intersection<G1::dimensions()>> intersections(const G1& geom1,const G2& geom2) noexcept;

    template<uint64_t DIMENSION>
    struct Intersections
    {
        template<typename G1,typename G2>
        std::vector<Intersection<G1::dimensions()>> operator()(const G1& geom1,const G2& geom2) 
            const noexcept(noexcept(intersections(geom1,geom2)))
        {
            return intersections(geom1,geom2);
        }
    };
}