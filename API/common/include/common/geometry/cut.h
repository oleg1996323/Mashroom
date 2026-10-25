#pragma once
#include <vector>
#include "intersections.h"

namespace geometry{
    template<typename G_CUTTED,typename G_CUTTING>
    std::vector<G_CUTTED> cut(
        const G_CUTTED& cutted,
        const G_CUTTING& cutting,
        bool left_forward);

    template<uint64_t DIMENSION>
    struct Cut
    {
        template<typename G1, typename G2>
        std::vector<G1> operator()(
            const G1& geom1,
            const G2& geom2,
            bool left_forward) 
            const noexcept(noexcept(cut(geom1,geom2,left_forward)))
        {
            return cut(geom1,geom2,left_forward);
        }
    };
}