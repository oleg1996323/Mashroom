#pragma once
#include <OsterLib/types/coord.h>

long double cross(const Coord& a, const Coord& b, const Coord& c);
bool on_segment(const Coord& a, const Coord& b, const Coord& c);
bool intersects(const Coord& first_1, const Coord& second_1, const Coord& first_2,const Coord& second_2) noexcept;