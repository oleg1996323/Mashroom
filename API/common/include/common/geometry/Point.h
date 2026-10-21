#pragma once
#include <type_traits>
#include <string_view>
#include "BasicGeometry.h"

namespace geometry{
template<typename POINT_T>
void print(const POINT_T& point,std::string_view desc="",bool newline = false);

template<typename POINT_T>
void print(const POINT_T& point, std::string_view desc = "", bool newline = false)
{
    if (!desc.empty())
        std::cout << desc.data() << ": ";
    std::cout << "(";

    [&]<std::uint64_t... Is>(std::index_sequence<Is...>) {
        ((
            std::cout
                << (Is == 0 ? "" : ",")
                << geometry::Get<Is, POINT_T>{}(point)
        ), ...);
    }(std::make_index_sequence<geometry::dimensions<POINT_T>()>());

    std::cout << ")";
    if (newline)
        std::cout << std::endl;
}

//specified point
template<typename POINT_T,uint64_t DIMENSIONS>
class BasicPoint;

template<typename POINT_T,uint64_t DIMENSIONS>
class BasicPointRef;

//specified point
template<typename POINT_T,uint64_t DIMENSIONS = dimensions<POINT_T>()>
class BasicPoint:public BasicGeometry<false,DIMENSIONS>{
    POINT_T data_;
    public:
    BasicPoint(const BasicPoint& other)
    {
        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
            (data_[Is]=other.data_[Is],...);
        }(std::make_index_sequence<BasicGeometry::dimensions()>());
    }
    BasicPoint(BasicPoint&& other) noexcept
    {
        if(this!=&other)
            data_.swap(other.data_);
    }
    BasicPoint(const BasicPointRef<POINT_T,DIMENSIONS>& point);
    BasicPoint(BasicPointRef<POINT_T,DIMENSIONS>&& point);
    BasicPoint& operator=(const BasicPoint& other){
        if(this!=&other)
            [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                (data_[Is]=other.data_[Is],...);
            }(std::make_index_sequence<BasicGeometry::dimensions()>());
        return *this;
    }
    BasicPoint& operator=(const BasicPointRef<POINT_T,DIMENSIONS>& other);
    BasicPoint(POINT_T point)
    {
        static_assert(BasicGeometry::dimensions()>0,"Not all Get::operator() are defined");
        [this]<uint64_t... Is>(std::index_sequence<Is...> is){
            (data_[Is]=Get<Is,POINT_T>{}(),...);
        }(std::make_index_sequence<BasicGeometry::dimensions()>());
    }
    template<uint64_t DIMENSION>
    VALUE_T get() const noexcept{
        static_assert(DIMENSION<BasicGeometry::dimensions());
        return geometry::Get<DIMENSION,BasicPoint>()(*this);
    }
};

//default array point
template<uint64_t DIMENSIONS,typename VALUE_T = long double>
using Point = BasicPoint<std::array<VALUE_T,DIMENSIONS>,DIMENSIONS>;

template<typename POINT_T,uint64_t DIMENSIONS>
class BasicPointRef:public BasicGeometry<true,DIMENSIONS>{
    const POINT_T& data_;
    public:
    BasicPointRef(const BasicPointRef& other):
        data_(other.data_)
    {}
    BasicPointRef(BasicPointRef&& other) = delete;
    BasicPointRef& operator=(const BasicPointRef& other){
        if(this!=&other){
            data_ = other.data_;
        }
        return *this;
    }
    BasicPointRef& operator=(BasicPointRef&& other) = delete;
    template<uint64_t DIMENSION>
    VALUE_T get() const noexcept{
        static_assert(DIMENSION<BasicGeometry::dimensions());
        return geometry::Get<DIMENSION,BasicPointRef>()(*this);
    }
};

//default array refpoint
template<uint64_t DIMENSIONS,typename VALUE_T>
using PointRef = BasicPointRef<std::array<VALUE_T,DIMENSIONS>>;

template<typename POINT_T,uint64_t DIMENSIONS>
BasicPoint<POINT_T,DIMENSIONS>::BasicPoint(const BasicPointRef<POINT_T,DIMENSIONS>& other)
{
    [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
        (data_[Is]=other[Is],...);
    }(std::make_index_sequence<BasicGeometry::dimensions()>());
}

template<typename POINT_T, uint64_t DIMENSIONS>
BasicPoint<POINT_T,DIMENSIONS>::BasicPoint(BasicPointRef<POINT_T,DIMENSIONS>&& other)
{
    data_.swap(other.data_);
}

template<typename POINT_T,uint64_t DIMENSIONS>
BasicPoint<POINT_T,DIMENSIONS>& BasicPoint<POINT_T,DIMENSIONS>::operator=(const BasicPointRef<POINT_T,DIMENSIONS>& other){
    if(this!=&other)
        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
            (data_[Is]=other.data_[Is],...);
        }(std::make_index_sequence<BasicGeometry::dimensions()>());
    return *this;
}

template<>
struct geometry::Get<0ul,Coord>{
    long double operator()(const Coord& coordinate) const noexcept{
        return coordinate.lon_;
    }
};

template<>
struct geometry::Get<1ul,Coord>{
    long double operator()(const Coord& coordinate) const noexcept{
        return coordinate.lat_;
    }
};

struct UserPoint{
    double X_;
    double Y_;
    UserPoint() = default;
    UserPoint(const UserPoint&) = default;
    UserPoint(UserPoint&&) = default;
    UserPoint& operator=(const UserPoint&) = default;
    UserPoint& operator=(UserPoint&&) = default;
    UserPoint(const Coord& coord):
        X_(coord.lon_),Y_(coord.lat_){}
    UserPoint(Coord&& coord):
        X_(coord.lon_),Y_(coord.lat_){}
    UserPoint& operator=(const Coord& coord)
    {
        X_=coord.lon_;
        Y_=coord.lat_;
        return *this;
    }
    UserPoint& operator=(Coord&& coord)
    {
        X_=coord.lon_;
        Y_=coord.lat_;
        return *this;
    }
};

template<>
struct geometry::Get<0ul,UserPoint>{
    long double operator()(const UserPoint& coordinate) const noexcept{
        return coordinate.X_;
    }
};

template<>
struct geometry::Get<1ul,UserPoint>{
    long double operator()(const UserPoint& coordinate) const noexcept{
        return coordinate.Y_;
    }
};

}