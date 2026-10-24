#pragma once
#include <type_traits>
#include <string_view>
#include "BasicGeometry.h"
#include "OsterLib/concepts.h"
#include <iostream>

namespace geometry{
    template<uint64_t DIMENSION,typename POINT_T>
    const auto& get(const POINT_T& point);

    template<uint64_t DIMENSION,typename POINT_T>
    auto& get(POINT_T& point);

    template<template<uint64_t D,typename... ARGS> 
            typename F,uint64_t D,typename... ARGS>
    std::invoke_result_t<F<D>,ARGS...> exec(ARGS&&... args) 
        noexcept(noexcept(F<D>()(std::forward<ARGS>(args)...)))
    {
        return F<D>()(std::forward<ARGS>(args)...);
    }

    template<uint64_t DIMENSION>
    struct Get{
        template<typename POINT_T>
        auto& operator()(POINT_T& point)
            noexcept(noexcept(get<DIMENSION>(point)))
        {
            return get<DIMENSION>(point);
        };
        template<typename POINT_T>
        const auto& operator()(const POINT_T& point) const 
            noexcept(noexcept(get<DIMENSION>(point)))
        {
            return get<DIMENSION>(point);
        };
    };

    template<uint64_t DIMENSION>
    struct Set{
        template<typename POINT_T,typename VALUE_T>
        void operator()(POINT_T& point,VALUE_T value){
            get<DIMENSION>(point) = value;
        }
    };

    template<uint64_t DIMENSION,typename POINT_T>
    struct defined_get_coordinate<DIMENSION,POINT_T,
    std::void_t<typename std::invoke_result<Get<DIMENSION>,POINT_T>::type>>:std::true_type{};

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
    void print(const POINT_T& point, std::string_view desc = "", bool newline = false)
    {
        if (!desc.empty())
            std::cout << desc.data() << ": ";
        std::cout << "(";

        [&]<std::uint64_t... Is>(std::index_sequence<Is...>) {
            ((
                std::cout
                    << (Is == 0 ? "" : ",")
                    << exec<Get,Is>(point)
            ), ...);
        }(std::make_index_sequence<geometry::dimensions<POINT_T>()>());

        std::cout << ")";
        if (newline)
            std::cout << std::endl;
    }

    //specified point
    template<uint64_t DIMENSIONS,typename VALUE_T>
    class BasicPoint;

    template<uint64_t DIMENSIONS,typename VALUE_T>
    class BasicPointRef;

    //specified point
    template<uint64_t DIMENSIONS,typename VALUE_T = long double>
    class BasicPoint:public BasicGeometry<false,DIMENSIONS>{
        std::array<VALUE_T,DIMENSIONS> data_;
        public:
        BasicPoint() = default;
        BasicPoint(std::initializer_list<VALUE_T> values):
            data_(values.begin(),values.end()){}
        BasicPoint(const BasicPoint& other)
        {
            [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                ((data_[Is]=other.data_[Is]),...);
            }(std::make_index_sequence<BasicGeometry<false,DIMENSIONS>::dimensions()>());
        }
        BasicPoint(BasicPoint&& other) noexcept
        {
            if(this!=&other)
                data_.swap(other.data_);
        }
        
        BasicPoint& operator=(const BasicPoint& other){
            if(this!=&other){
                [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
                    ((data_[Is]=other.data_[Is]),...);
                }(std::make_index_sequence<
                    BasicGeometry<false, DIMENSIONS>::dimensions()>());
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
        BasicPoint& operator=(BasicPointRef<DIMENSIONS,VALUE_T>&& other) noexcept;
        bool operator==(const BasicPoint<DIMENSIONS,VALUE_T>& other) const{
            auto cmp = [this,&other]<uint64_t... Is>(std::index_sequence<Is...>)
            {
                return ((get<Is>()==other.get<Is>()) && ...);
            };
            return cmp(std::make_index_sequence<DIMENSIONS>());
        }
        template<uint64_t DIMENSION>
        const auto& get() const noexcept{
            static_assert(DIMENSION<
                BasicGeometry<false, DIMENSIONS>::dimensions(),
            "Not existing dimension");
            return data_[DIMENSION];
        }
        template<uint64_t DIMENSION>
        auto& get() noexcept{
            static_assert(DIMENSION<
                BasicGeometry<false, DIMENSIONS>::dimensions(),
            "Not existing dimension");
            return data_[DIMENSION];
        }
        template<uint64_t DIMENSION,typename ARG>
        void set(ARG value) noexcept{
            static_assert(DIMENSION<
                BasicGeometry<false, DIMENSIONS>::dimensions(),
            "Not existing dimension");
            data_[DIMENSION] = value;
        }
    };

    template<uint64_t DIMENSION,uint64_t DIMENSIONS,typename VALUE_T>
    const auto& get(const BasicPoint<DIMENSIONS,VALUE_T>& point){
        return point.template get<DIMENSION>();
    }

    template<uint64_t DIMENSION,uint64_t DIMENSIONS,typename VALUE_T>
    auto& get(BasicPoint<DIMENSIONS,VALUE_T>& point){
        return point.template get<DIMENSION>();
    }

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
            static_assert(DIMENSION<
                BasicGeometry<false, DIMENSIONS>::dimensions(),
            "Not existing dimension");
            return data_[DIMENSION];
        }
    };

    template<uint64_t DIMENSION,uint64_t DIMENSIONS,typename VALUE_T>
    const auto& get(const BasicPointRef<DIMENSIONS,VALUE_T>& point){
        return point.template get<DIMENSION>();
    }

    template<uint64_t DIMENSIONS,typename VALUE_T>
    BasicPoint<DIMENSIONS,VALUE_T>::BasicPoint(const BasicPointRef<DIMENSIONS,VALUE_T>& other)
    {
        
        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
            ((set<Is>(other.template get<Is>())),...);
        }(std::make_index_sequence<BasicGeometry<false,DIMENSIONS>::dimensions()>());
    }

    template<uint64_t DIMENSIONS,typename VALUE_T>
    BasicPoint<DIMENSIONS,VALUE_T>::BasicPoint(BasicPointRef<DIMENSIONS,VALUE_T>&& other) noexcept
    {
        data_.swap(other.data_);
    }

    template<uint64_t DIMENSIONS,typename VALUE_T>
    BasicPoint<DIMENSIONS,VALUE_T>& 
        BasicPoint<DIMENSIONS,VALUE_T>::operator=(
            const BasicPointRef<DIMENSIONS,VALUE_T>& other)
    {
        [this,&other]<uint64_t... Is>(std::index_sequence<Is...> is){
            ((set<Is>(other.template get<Is>())),...);
        }(std::make_index_sequence<BasicGeometry<false,DIMENSIONS>::dimensions()>());
        return *this;
    }

    template<>
    const auto& get<0>(const Coord& point){
        return point.lon_;
    }

    template<>
    const auto& get<1>(const Coord& point){
        return point.lat_;
    }

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
        long double dX = static_cast<long double>(exec<Get,0>(second)) - exec<Get,0>(first);
        long double dY = static_cast<long double>(exec<Get,1>(second)) - exec<Get,1>(first);
        return std::sqrt(dX*dX+dY*dY);
    }
}