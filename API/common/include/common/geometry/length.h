#pragma once


namespace geometry{
    template<typename GEOM>
    auto length(const GEOM& geom);

    template<typename GEOM>
    struct Length
    {
        bool operator()(const GEOM& geom) 
            const noexcept(noexcept(length(geom)))
        {
            return length(geom);
        }
    };
}