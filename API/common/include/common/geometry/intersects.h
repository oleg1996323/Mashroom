#pragma once


namespace geometry{
    template<typename G1,typename G2>
    bool intersects(const G1& geom1,const G2& geom2) noexcept;

    template<uint64_t DIMENSION>
    struct Intersects
    {
        template<typename G1, typename G2>
        bool operator()(const G1& geom1,const G2& geom2) 
            const noexcept(noexcept(intersects(geom1,geom2)))
        {
            return intersects(geom1,geom2);
        }
    };
}