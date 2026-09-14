#pragma once
#include <type_traits>
#include "OsterLib/types/coord.h"
#include <boost/geometry.hpp>

namespace boost
{
    namespace geometry
    {
        namespace traits
        {
            template<> struct tag<Coord>
            { typedef point_tag type; };

            template<> struct coordinate_type<Coord>
            { typedef double type; };

            template<> struct coordinate_system<Coord>
            { typedef cs::cartesian type; };

            template<> struct dimension<Coord> : boost::mpl::int_<2> {};

            template<>
            struct access<Coord, 0>
            {
                static Lon get(Coord const& p)
                {
                    return p.lon_;
                }

                static void set(Coord& p, Lon value)
                {
                    p.lon_ = value;
                }
            };

            template<>
            struct access<Coord, 1>
            {
                static Lat get(Coord const& p)
                {
                    return p.lat_;
                }

                static void set(Coord& p, Lat value)
                {
                    p.lat_ = value;
                }
            };
        }
    }
}

struct Point{
    double X_,Y_= 0;
};

namespace boost
{
    namespace geometry
    {
        namespace traits
        {
            template<> struct tag<Point>
            { typedef point_tag type; };

            template<> struct coordinate_type<Point>
            { typedef double type; };

            template<> struct coordinate_system<Point>
            { typedef cs::cartesian type; };

            template<> struct dimension<Point> : boost::mpl::int_<2> {};

            template<>
            struct access<Point, 0>
            {
                static double get(Point const& p)
                {
                    return p.X_;
                }

                static void set(Point& p, double value)
                {
                    p.X_ = value;
                }
            };

            template<>
            struct access<Point, 1>
            {
                static double get(Point const& p)
                {
                    return p.Y_;
                }

                static void set(Point& p, double value)
                {
                    p.Y_ = value;
                }
            };
        }
    }
}