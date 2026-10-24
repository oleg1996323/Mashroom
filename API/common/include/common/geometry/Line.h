#pragma once
#include <cmath>
#include <numbers>
#include <system_error>
#include <optional>
#include "algorithms.h"
#include "BasicGeometry.h"
#include "Point.h"
#include "cut.h"
#include "intersects.h"


namespace geometry{
    template<uint64_t DIMENSIONS,typename VALUE_T = long double>
    class BasicLine:public BasicGeometry<false,DIMENSIONS>{
        BasicPoint<DIMENSIONS,VALUE_T> first_;
        BasicPoint<DIMENSIONS,VALUE_T> second_;
        public:
        using id = uint64_t;
        BasicLine() = default;
        BasicLine(Point<DIMENSIONS,VALUE_T> first,
            Point<DIMENSIONS,VALUE_T> second) noexcept:
        first_(std::move(first)),second_(second){}
        BasicLine(BasicPoint<DIMENSIONS,VALUE_T>&& first,
                BasicPoint<DIMENSIONS,VALUE_T>&& second) noexcept:
        first_(std::move(first)),second_(std::move(second)){}
        BasicLine(const BasicLine& other) = default;
        BasicLine(BasicLine&& other) noexcept = default;
        BasicLine& operator=(const BasicLine& other) = default;
        BasicLine& operator=(BasicLine&& other) noexcept = default;
        bool operator==(const BasicLine&) = delete;
        template<typename OTHER_POINT_T,uint64_t OTHER_DIMENSIONS>
        const Point<DIMENSIONS,VALUE_T>& first() const noexcept{
            return first_;
        }
        const Point<DIMENSIONS,VALUE_T>& second() const noexcept{
            return second_;
        }
        double k() const noexcept{
            return k(first_.template get<0>(),
                first_.template get<1>(),
                second_.template get<0>(),
                second_.template get<1>());
        }
        static double k(
                long double X1,
                long double Y1,
                long double X2,
                long double Y2) noexcept
        {
            long double dX = X2- X1;
            if(std::abs(dX)<std::numeric_limits<double>::epsilon()){
                return std::numeric_limits<double>::infinity();
            }
            long double dY = Y2 - Y1;
            return dY/dX;
        }
        double angle() const noexcept{
            return angle(first_.template get<0>(),
                first_.template get<1>(),
                second_.template get<0>(),
                second_.template get<1>());
        }
        static double angle(
                long double X1,
                long double Y1,
                long double X2,
                long double Y2) noexcept
        {
            return atan2l(Y2-Y1,X2-X1);
        }
        bool is_vertical() const noexcept{
            double k_ = k();
            return std::isinf(k_);
        }
        bool is_horizontal() const noexcept{
            return std::abs(static_cast<long double>(second_.template get<1>()) - 
                first_.template get<1>()) < 
                    std::numeric_limits<double>::epsilon();
        }
        static bool straight_above_the_point(
                const BasicLine& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            double X0 = line.first().template get<1>() - k*line.first().template get<0>();
            return Y<k*X+X0;
        }
        static bool straight_under_the_point(
                const BasicLine& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            long double X0 = line.first().template get<1>() - k*line.first().template get<0>();
            return Y>k*X+X0;
        }
        static bool point_on_straight(
                const BasicLine& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            long double X0 = line.first().template get<1>() - k*line.first().template get<0>();
            return std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
        }
        static bool straight_not_under_the_point(
                const BasicLine& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            long double X0 = line.first().template get<1>() - k*line.first().template get<0>();
            return Y<k*X+X0 || std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
        }
        static bool straight_not_above_the_point(
                const BasicLine& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            long double X0 = line.first().template get<1>() - k*line.first().template get<0>();
            return Y>k*X+X0 || std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
        }
        bool intersects(const BasicLine& other) const noexcept{
            return exec<Intersects,2>(*this,other);
        }
        static BasicLine cut(
                const BasicLine& line,
                const BasicLine& other,
                bool left_forward
                ) noexcept
        {
            return exec<Cut,DIMENSIONS>(line,other,left_forward);
        }
    };

    template<typename VALUE1_T,typename VALUE2_T>
    bool equivalent(const BasicLine<2,VALUE1_T>& first,
                    const BasicLine<2,VALUE1_T>& second){
        using value_result_t = std::common_type_t<VALUE1_T,VALUE2_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        if(&first!=&second){
            return std::abs(static_cast<max_prec_t>(
                    first.first().template get<0>())-
                    second.first().template get<0>())<
                    std::numeric_limits<max_prec_t>::epsilon() &&
            std::abs(static_cast<max_prec_t>(
                    first.second().template get<0>())-
                    second.second().template get<0>())<
                    std::numeric_limits<max_prec_t>::epsilon() &&
            std::abs(static_cast<max_prec_t>(
                    first.first().template get<1>())-
                    second.first().template get<1>())<
                    std::numeric_limits<max_prec_t>::epsilon() &&
            std::abs(static_cast<max_prec_t>(
                    first.second().template get<1>())-
                    second.second().template get<1>())<
                    std::numeric_limits<max_prec_t>::epsilon();
        }
        else return true;
    }

    template<typename VALUE1_T,typename VALUE2_T>
    std::vector<Intersection<2>> intersections(
        const BasicLine<2,std::common_type_t<VALUE1_T,VALUE2_T>>& first,
        const BasicLine<2,std::common_type_t<VALUE1_T,VALUE2_T>>& second) noexcept
    {
        if(&first==&second)
            return {};
        using value_result_t = std::common_type_t<VALUE1_T,VALUE2_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        max_prec_t X11,X12,X21,X22,Y11,Y12,Y21,Y22;
        X11 = first.first().template get<0>();
        X12 = first.second().template get<0>();
        Y11 = first.first().template get<1>();
        Y12 = first.second().template get<1>();
        X21 = second.first().template get<0>();
        X22 = second.second().template get<0>();
        Y21 = second.first().template get<1>();
        Y22 = second.second().template get<1>();
        max_prec_t scale = std::max(std::abs(X12 - X11), std::abs(Y12 - Y11))
            * std::max(std::abs(X22 - X21), std::abs(Y22 - Y21));
        max_prec_t denominator = (Y22 - Y21) * (X12 - X11) - (X22 - X21) * (Y12 - Y11);
        if (scale == 0.0 || std::abs(denominator) < 1e-12 * scale)
            return {};
        max_prec_t u_a = ((X22 - X21) * (Y11 - Y21) - (Y22 - Y21) * (X11 - X21)) / denominator;
        return {Intersection<2>(Point<2>({X11+u_a*(X12-X11),Y11+u_a*(Y12 - Y11)}),0,0)};
    }

    template<uint64_t DIMENSIONS,typename VALUE_T = long double>
    VALUE_T length(const BasicLine<DIMENSIONS,VALUE_T>& line){
        exec<Distance,DIMENSIONS>(line.first(),line.second());
    }


    template<typename VALUE1_T,typename VALUE2_T>
    std::vector<BasicLine<
            2,
            std::common_type_t<VALUE1_T,VALUE2_T>>> cut(
        const BasicLine<2,VALUE1_T>& cutted,
        const BasicLine<2,VALUE2_T>& cutting,
        bool left_forward)
    {
        using value_result_t = std::common_type_t<VALUE1_T,VALUE2_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        auto point = exec<Intersections,2>(cutted,cutting);
        if(point.empty())
            return {cutted};
        Point<2,max_prec_t> intersection_point
            ({point.front().point().template get<0>(),
                point.front().point().template get<1>()});
        if(!geometry::on_segment(cutted.first(),cutted.second(),intersection_point))
            return {cutted};
        long double side = cross(cutting.first(), cutting.second(), cutted.first());
        if(left_forward){
            if(side>0)
                return {BasicLine(intersection_point,cutted.first())};
            else
                return {BasicLine(cutted.second(),intersection_point)};
        }
        else{
            if(side>0)
                return {BasicLine(cutted.second(),intersection_point)};
            else
                return {BasicLine(intersection_point,cutted.first())};
        }
    }

    template<typename VALUE1_T,typename VALUE2_T>
    bool intersects(const BasicLine<2,VALUE1_T>& first, const BasicLine<2,VALUE2_T>& second) noexcept{
        long double d1 = cross(first.first(), first.second(), second.first());
        long double d2 = cross(first.first(), first.second(), second.second());
        long double d3 = cross(first.second(), second.second(), first.first());
        long double d4 = cross(first.second(), second.second(), second.first());
        if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
            ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
            return true;
        if (d1 == 0 && on_segment(first.first(), second.first(), first.second())) return true;
        if (d2 == 0 && on_segment(first.first(), second.first(), second.second())) return true;
        if (d3 == 0 && on_segment(first.second(), second.second(), first.first())) return true;
        if (d4 == 0 && on_segment(first.second(), second.second(), second.first())) return true;
        return false;
    }
}