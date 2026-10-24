#pragma once


namespace geometry{
    template<typename G1,typename G2>
    bool intersects(const G1& geom1,const G2& geom2) noexcept;

    template<typename G1, typename G2>
    struct Intersects
    {
        bool operator()(const G1& geom1,const G2& geom2) 
            const noexcept(noexcept(intersects(geom1,geom2)))
        {
            return intersects(geom1,geom2);
        }
    };
}