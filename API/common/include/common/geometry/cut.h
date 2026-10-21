#pragma once
#include <vector>
#include "intersections.h"

namespace geometry{
    template<typename G_CUTTED,typename G_CUTTING>
    std::vector<G_CUTTED> cut(
        const G_CUTTED& cutted,
        const G_CUTTING& cutting,
        bool left_forward);

    template<typename G1, typename G2>
    struct GetCut
    {
        std::vector<G1> operator()(const G1& geom1,const G2& geom2) 
            const noexcept(noexcept(cut(geom1,geom2)))
        {
            return cut(geom1,geom2);
        }
    };
}