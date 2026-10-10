#include "common/geometry/algorithms.h"

long double cross(const Coord& a, const Coord& b, const Coord& c) {
    return (static_cast<long double>(b.lon_) - 
            static_cast<long double>(a.lon_)) * 
            (static_cast<long double>(c.lat_) - 
            static_cast<long double>(a.lat_))
        - (static_cast<long double>(b.lat_) - 
            static_cast<long double>(a.lat_)) * 
            (static_cast<long double>(c.lon_) - 
            static_cast<long double>(a.lon_));
}

bool on_segment(const Coord& a, const Coord& b, const Coord& c) {
    return std::min(a.lon_, b.lon_) <= c.lon_ && c.lon_ <= std::max(a.lon_, b.lon_)
        && std::min(a.lat_, b.lat_) <= c.lat_ && c.lat_ <= std::max(a.lat_, b.lat_);
}

bool intersects(const Coord& first_1, const Coord& second_1, const Coord& first_2,const Coord& second_2) noexcept{
    long double d1 = cross(first_1, second_1, first_2);
    long double d2 = cross(first_1, second_1, second_2);
    long double d3 = cross(first_2, second_2, first_1);
    long double d4 = cross(first_2, second_2, second_1);
    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
        return true;
    if (d1 == 0 && on_segment(first_1, second_1, first_2)) return true;
    if (d2 == 0 && on_segment(first_1, second_1, second_2)) return true;
    if (d3 == 0 && on_segment(first_2, second_2, first_1)) return true;
    if (d4 == 0 && on_segment(first_2, second_2, second_1)) return true;
    return false;
}