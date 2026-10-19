#pragma once
#include <type_traits>
#include <string_view>
#include "BasicGeometry.h"


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

template<uint64_t DIMENSIONS>
class BasicPoint;

template<uint64_t DIMENSIONS>
class BasicPointRef;

template<uint64_t DIMENSIONS>
class BasicPoint:public BasicGeometry<false,DIMENSIONS>{
    std::array<long double,DIMENSIONS> data_;
    public:
    BasicPoint(const BasicPoint& other)
    {
        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
            (data_[Is]=other.data_[Is],...);
        }(std::make_index_sequence<DIMENSIONS>());
    }
    BasicPoint(BasicPoint&& other) noexcept
    {
        if(this!=&other)
            data_.swap(other.data_);
    }
    BasicPoint(const BasicPointRef<DIMENSIONS>& point);
    BasicPoint(BasicPointRef<DIMENSIONS>&& point);
    BasicPoint& operator=(const BasicPoint& other){
        if(this!=&other)
            [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                (data_[Is]=other.data_[Is],...);
            }(std::make_index_sequence<DIMENSIONS>());
        return *this;
    }
    BasicPoint& operator=(const BasicPointRef<DIMENSIONS>& other);
    template<typename POINT_T>
    BasicPoint(POINT_T point)-> BasicPoint<dimensions<POINT_T>()>()
    {
        static_assert(dimensions<POINT_T>()>0,"Not all Get::operator() are defined");
        [this]<uint64_t... Is>(std::index_sequence<Is...> is){
            (data_[Is]=Get<Is,POINT_T>{}(),...);
        }(std::make_index_sequence<dimensions<POINT_T>()>());
    }
    template<uint64_t DIMENSION>
    long double get() const noexcept{
        static_assert(DIMENSION<DIMENSIONS);
        return geometry::Get<DIMENSION,BasicPoint>()(*this);
    }
};

template<uint64_t DIMENSIONS>
class BasicPointRef:public BasicGeometry<true,DIMENSIONS>{
    const std::array<long double,DIMENSIONS>& data_;
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
    long double get() const noexcept{
        static_assert(DIMENSION<DIMENSIONS);
        return geometry::Get<DIMENSION,BasicPointRef>()(*this);
    }
};

template<uint64_t DIMENSIONS>
BasicPoint<DIMENSIONS>::BasicPoint(const BasicPointRef<DIMENSIONS>& other)
{
    [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
        (data_[Is]=other[Is],...);
    }(std::make_index_sequence<DIMENSIONS>());
}

template<uint64_t DIMENSIONS>
BasicPoint<DIMENSIONS>::BasicPoint(BasicPointRef<DIMENSIONS>&& other)
{
    data_.swap(other.data_);
}

template<uint64_t DIMENSIONS>
BasicPoint<DIMENSIONS>& BasicPoint<DIMENSIONS>::operator=(const BasicPointRef<DIMENSIONS>& other){
    if(this!=&other)
        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
            (data_[Is]=other.data_[Is],...);
        }(std::make_index_sequence<DIMENSIONS>());
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

inline void foo(){
    geometry::BasicPoint<2>(Coord()).get<uint64_t(0)>();
}