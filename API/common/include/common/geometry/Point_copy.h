#pragma once
#include <type_traits>
#include <string_view>
#include "BasicGeometry.h"

namespace geometry{
    template<uint64_t DIMENSION,typename POINT_T>
    struct Get;

    template<uint64_t DIMENSION, typename POINT_T>
    struct Set;

    template<uint64_t DIMENSION,typename POINT_T>
    struct defined_get_coordinate<DIMENSION,POINT_T,
    std::void_t<typename std::invoke_result<Get<DIMENSION,POINT_T>,POINT_T>::type>>:std::true_type{};

    template<uint64_t DIMENSION,typename POINT_T>
    struct Get{
        const auto& operator()(const POINT_T& point) const = delete;
        auto& operator()(POINT_T& point) = delete;
    };

    template<uint64_t DIMENSION, typename POINT_T>
    struct Set{
        void operator()(POINT_T& point,auto value){
            Get<DIMENSION,POINT_T>()(ref_) = value;
        }
    };

    template<typename POINT_T, uint64_t D = 0, uint64_t MAX = MAX_ALLOWABLE_DIMENSIONS>
    consteval uint64_t dimensions() noexcept {
        static_assert(D<=MAX,MAX_ALLOWABLE_DIMENSIONS_STR(MAX_ALLOWABLE_DIMENSIONS));
        if constexpr (D >= MAX)
            return MAX;
        else if constexpr (defined_get_coordinate<D, POINT_T>::value)
            return dimensions<POINT_T, D + 1, MAX>();
        else
            return D;
    }

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
                    << geometry::Get<Is>(point)()
            ), ...);
        }(std::make_index_sequence<geometry::dimensions<POINT_T>()>());

        std::cout << ")";
        if (newline)
            std::cout << std::endl;
    }

    //specified point
    template<uint64_t DIMENSIONS,typename VALUE_T = long double>
    class BasicPoint;

    template<uint64_t DIMENSIONS,typename VALUE_T = long double>
    class BasicPointRef;

    //specified point
    template<uint64_t DIMENSIONS,typename VALUE_T = long double>
    class BasicPoint:public BasicGeometry<false,DIMENSIONS>{
        std::array<VALUE_T,DIMENSIONS> data_;
        public:
        BasicPoint() = default;
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
        
        BasicPoint& operator=(const BasicPoint& other){
            if(this!=&other){
                [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                    (data_[Is]=other.data_[Is],...);
                }(std::make_index_sequence<BasicGeometry::dimensions()>());
            }
            return *this;
        }
        BasicPoint& operator=(BasicPoint&& other){
            if(this!=&other)
                data_.swap(other.data_);
            return *this;
        }
        BasicPoint(const BasicPointRef<DIMENSIONS,VALUE_T>& point);
        BasicPoint(BasicPointRef<DIMENSIONS,VALUE_T>&& point) noexcept;
        BasicPoint& operator=(const BasicPointRef<DIMENSIONS,VALUE_T>& other);
        BasicPoint& operator=(BasicPoint<DIMENSIONS,VALUE_T>&& other) noexcept;
        template<uint64_t DIMENSION>
        const auto& get() const noexcept{
            static_assert(DIMENSION<BasicGeometry::dimensions(),
            "Not existing dimension");
            return data_[DIMENSION];
        }
        template<uint64_t DIMENSION>
        auto& get() noexcept{
            static_assert(DIMENSION<BasicGeometry::dimensions(),
            "Not existing dimension");
            return data_[DIMENSION];
        }
        template<uint64_t DIMENSION,typename VALUE_T>
        void set(VALUE_T value) noexcept{
            static_assert(DIMENSION<BasicGeometry::dimensions(),
            "Not existing dimension");
            data_[DIMENSION] = value;
        }
    };

    template<uint64_t DIMENSION,uint64_t DIMENSIONS,typename VALUE_T>
    struct Get<DIMENSION,BasicPoint<DIMENSIONS,VALUE_T>>{
        const auto& operator()(const BasicPoint<DIMENSIONS,VALUE_T>& point) const noexcept{
            return point.get<DIMENSION>();
        }
        auto& operator()(const BasicPoint<DIMENSIONS,VALUE_T>& point) noexcept{
            return point.get<DIMENSION>();
        }
    };

    template<uint64_t DIMENSIONS,typename VALUE_T = long double>
    class BasicPointRef:public BasicGeometry<true,DIMENSIONS>{
        std::array<VALUE_T,DIMENSIONS>& data_;
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
        const VALUE_T& get() const noexcept{
            static_assert(DIMENSION<BasicGeometry::dimensions(),
            "Not existing dimension");
            return data_[DIMENSION];
        }
        template<uint64_t DIMENSION>
        VALUE_T& get() noexcept{
            static_assert(DIMENSION<BasicGeometry::dimensions(),
            "Not existing dimension");
            return data_[DIMENSION];
        }
        template<uint64_t DIMENSION,typename VALUE_T>
        void set(VALUE_T value) noexcept{
            static_assert(DIMENSION<BasicGeometry::dimensions(),
            "Not existing dimension");
            data_[DIMENSION] = value;
        }
    };

    template<uint64_t DIMENSION,uint64_t DIMENSIONS,typename VALUE_T>
    struct Get<DIMENSION,BasicPoint<DIMENSIONS,VALUE_T>>{
        const auto& operator()(const BasicPointRef<DIMENSIONS,VALUE_T>& point) const noexcept{
            return point.get<DIMENSION>();
        }
        auto& operator()(const BasicPointRef<DIMENSIONS,VALUE_T>& point) noexcept{
            return point.get<DIMENSION>();
        }
    };

    template<uint64_t DIMENSIONS,typename VALUE_T>
    BasicPoint<DIMENSIONS,VALUE_T>::BasicPoint(const BasicPointRef<DIMENSIONS,VALUE_T>& other)
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

    template<uint64_t DIMENSIONS,typename VALUE_T>
    BasicPoint<DIMENSIONS,VALUE_T>::BasicPoint(BasicPointRef<DIMENSIONS,VALUE_T>&& other) noexcept
    {
        data_.swap(other.data_);
        return *this;
    }

    template<uint64_t DIMENSIONS,typename VALUE_T>
    BasicPoint<DIMENSIONS,VALUE_T>& 
        BasicPoint<DIMENSIONS,VALUE_T>::operator=(
            const BasicPointRef<DIMENSIONS,VALUE_T>& other)
    {
        if(this!=&other)
            [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                (data_[Is]=other.data_[Is],...);
            }(std::make_index_sequence<BasicGeometry::dimensions()>());
        return *this;
    }

    template<>
    struct Get<DIMENSION,Coord>{
        const auto& operator()(const Coord& point) const noexcept{
            return point.lon_;
        }
        auto& operator()(const Coord& point) noexcept{
            return point.lat_;
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
    const auto& get<0>(const UserPoint& point){
        return point.X_;
    }

    template<>
    const auto& get<1>(const UserPoint& point){
        return point.Y_;
        BasicPoint<2> p;
    }
    static_assert(std::is_default_constructible_v<BasicPoint<2>>);
    static_assert(std::is_copy_constructible_v<BasicPoint<2>>);
    static_assert(std::is_move_constructible_v<BasicPoint<2>>);
    static_assert(std::is_copy_assignable_v<BasicPoint<2>>);
    static_assert(std::is_move_assignable_v<BasicPoint<2>>);
    static_assert(std::is_assignable_v<BasicPoint<2>,const BasicPointRef<2>&>);
    static_assert(std::is_assignable_v<BasicPoint<2>,BasicPointRef<2>&&>);
    static_assert(!std::is_default_constructible_v<BasicPointRef<2>>);
    static_assert(std::is_copy_constructible_v<BasicPointRef<2>>);
    static_assert(!std::is_move_constructible_v<BasicPointRef<2>>);
    static_assert(std::is_copy_assignable_v<BasicPointRef<2>>);
    static_assert(!std::is_move_assignable_v<BasicPointRef<2>>);
    template<uint64_t DIMENSIONS,typename VALUE_T = long double>
    using Point = BasicPoint<DIMENSIONS,VALUE_T>;
}

#include "distance.h"

namespace geometry{
    template<>
    auto distance<Point<2>,Point<2>>(
        const Point<2>& first,
        const Point<2>& second) noexcept
    {
        long double dX = static_cast<long double>(Get<0>{}(second))- Get<0>()(first);
        long double dY = static_cast<long double>(Get<1>()(second))- Get<1>()(first);
        return std::sqrt(dX*dX+dY*dY);
    }
}