#pragma once
#include <type_traits>
#include <string_view>
#include "BasicGeometry.h"
#include "OsterLib/concepts.h"



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
        BasicPoint() = default;
        template<typename OTHER_POINT_T,uint64_t OTHER_DIMENSIONS>
        BasicPoint(const BasicPoint<OTHER_POINT_T,OTHER_DIMENSIONS>& other)
        {
            if constexpr(std::is_assignable_v<POINT_T,OTHER_POINT_T>)
                data_ = other.data_;
            else{   
                if constexpr (std::is_array_v<decltype(data_)>)
                    [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                        (data_[Is]=other.data_[Is],...);
                    }(std::make_index_sequence<BasicGeometry::dimensions()>());
                else
                    [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                        (data_[Is]=other.data_[Is],...);
                    }(std::make_index_sequence<Min<DIMENSIONS,OTHER_DIMENSIONS>>());
            }
        }
        BasicPoint(BasicPoint&& other) noexcept
        {
            if(this!=&other)
                data_.swap(other.data_);
        }
        BasicPoint(const BasicPointRef<POINT_T,DIMENSIONS>& point);
        BasicPoint(BasicPointRef<POINT_T,DIMENSIONS>&& point);
        template<typename OTHER_POINT_T,uint64_t OTHER_DIMENSIONS>
        BasicPoint& operator=(const BasicPoint<OTHER_POINT_T,OTHER_DIMENSIONS>& other){
            if(this!=&other){
                if constexpr(std::is_assignable_v<POINT_T,OTHER_POINT_T>)
                    data_ = other.data_;
                else{   
                    if constexpr (std::is_array_v<decltype(data_)>)
                        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                            (data_[Is]=other.data_[Is],...);
                        }(std::make_index_sequence<BasicGeometry::dimensions()>());
                    else
                        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                            (data_[Is]=other.data_[Is],...);
                        }(std::make_index_sequence<Min<DIMENSIONS,OTHER_DIMENSIONS>>());
                }
            }
            return *this;
        }
        template<typename OTHER_POINT_T,uint64_t OTHER_DIMENSIONS>
        BasicPoint& operator=(BasicPoint<OTHER_POINT_T,OTHER_DIMENSIONS>&& other){
            if(this!=&other){
                if constexpr(std::is_assignable_v<POINT_T,OTHER_POINT_T>)
                    data_ = std::move(other.data_);
                else{   
                    if constexpr (std::is_array_v<decltype(data_)>)
                        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                            (data_[Is]=other.data_[Is],...);
                        }(std::make_index_sequence<BasicGeometry::dimensions()>());
                    else
                        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                            (data_[Is]=other.data_[Is],...);
                        }(std::make_index_sequence<Min<DIMENSIONS,OTHER_DIMENSIONS>>());
                }
            }
            return *this;
        }
        template<typename OTHER_POINT_T,uint64_t OTHER_DIMENSIONS>
        BasicPoint& operator=(const BasicPointRef<OTHER_POINT_T,OTHER_DIMENSIONS>& other);
        BasicPoint(POINT_T point):data_(point){}
        template<uint64_t DIMENSION>
        auto get() const noexcept{
            static_assert(DIMENSION<BasicGeometry::dimensions(),
            "Not existing dimension");
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

    template<typename POINT_T,uint64_t DIMENSIONS,
        typename OTHER_POINT_T,uint64_t OTHER_DIMENSIONS>
    BasicPoint<POINT_T,DIMENSIONS>::BasicPoint(const BasicPointRef<POINT_T,OTHER_DIMENSIONS>& other)
    {
        if(this!=&other){
            if constexpr(std::is_assignable_v<POINT_T,OTHER_POINT_T>)
                data_ = other.data_;
            else{   
                if constexpr (std::is_array_v<decltype(data_)>)
                    [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                        (data_[Is]=other.data_[Is],...);
                    }(std::make_index_sequence<BasicGeometry::dimensions()>());
                else
                    [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                        (data_[Is]=other.data_[Is],...);
                    }(std::make_index_sequence<Min<DIMENSIONS,OTHER_DIMENSIONS>>());
            }
        }
        return *this;
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
    static_assert(std::is_default_constructible_v<Point<2>>);
    static_assert(std::is_copy_constructible_v<Point<2>>);
    static_assert(std::is_move_constructible_v<Point<2>>);
    static_assert(std::is_copy_assignable_v<Point<2>>);
    static_assert(std::is_move_assignable_v<Point<2>>);
}