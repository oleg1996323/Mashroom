#pragma once

namespace geometry{
    template<typename G1,typename G2>
    auto distance(const G1& geom1,const G2& geom2) noexcept;

    template<uint64_t DIMENSION>
    struct Distance
    {
        template<typename G1,typename G2>
        auto operator()(const G1& geom1,const G2& geom2) 
            const noexcept(noexcept(distance(geom1,geom2)))
        {
            return distance(geom1,geom2);
        }
    };
}