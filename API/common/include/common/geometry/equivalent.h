#pragma once
#include "BasicGeometry.h"

namespace geometry{
    template<typename G1,typename G2>
    bool equivalent(const G1& geom1,const G2& geom2) noexcept;

    template<typename G1, typename G2>
    struct Equivalent
    {
        bool operator()(const G1& geom1,const G2& geom2) 
            const noexcept(noexcept(equivalent<G1,G2>(geom1,geom2)))
        {
            return equivalent<G1,G2>(geom1,geom2);
        }
    };
}