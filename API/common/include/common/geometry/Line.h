#pragma once
#include <cmath>
#include <numbers>
#include <system_error>
#include <optional>
#include "algorithms.h"
#include "BasicGeometry.h"
#include "Point.h"

namespace geometry{
    template<typename POINT_T,uint64_t DIMENSIONS = dimensions<POINT_T>()>
    class BasicLine{
        BasicPoint<POINT_T> first_;
        BasicPoint<POINT_T> second_;
        public:
        using id = uint64_t;
        BasicLine() = default;
        BasicLine(POINT_T first,POINT_T second) noexcept:
        first_(std::move(first)),second_(second){}
        template<typename OTHER_POINT_T>
        BasicLine(BasicPoint<OTHER_POINT_T> first,BasicPoint<OTHER_POINT_T> second) noexcept:
        first_(std::move(first)),second_(second){}
        BasicLine(const BasicLine& other) = default;
        BasicLine(BasicLine&& other) noexcept = default;
        BasicLine& operator=(const BasicLine& other) = default;
        BasicLine& operator=(BasicLine&& other) noexcept = default;
        bool operator==(const BasicLine&) = delete;
        template<typename OTHER_POINT_T,uint64_t OTHER_DIMENSIONS>
        Coord first() const noexcept{
            return Coord{.lat_=Y1_,.lon_=X1_};
        }
        Coord second() const noexcept{
            return Coord{.lat_=Y2_,.lon_=X2_};
        }
        double X1() const noexcept{
            return X1_;
        }
        double Y1() const noexcept{
            return Y1_;
        }
        double X2() const noexcept{
            return X2_;
        }
        double Y2() const noexcept{
            return Y2_;
        }
        void X1(double X1) noexcept{
            X1_ = X1;
        }
        void X2(double X2) noexcept{
            X2_ = X2;
        }
        void Y1(double Y1) noexcept{
            Y1_ = Y1;
        }
        void Y2(double Y2) noexcept{
            Y2_ = Y2;
        }
        double length() const noexcept{
            long double dX = static_cast<long double>(X2_)- X1_;
            long double dY = static_cast<long double>(Y2_)- Y1_;
            return std::sqrt(dX*dX+dY*dY);
        }
        double k() const noexcept{
            return k(X1_,Y1_,X2_,Y2_);
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
            return angle(X1_,Y1_,X2_,Y2_);
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
            return std::abs(static_cast<long double>(Y2_) - Y1_) < std::numeric_limits<double>::epsilon();
        }
        static bool straight_above_the_point(
                const Line& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            double X0 = line.Y1() - k*line.X1();
            return Y<k*X+X0;
        }
        static bool straight_under_the_point(
                const Line& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            long double X0 = line.Y1() - k*line.X1();
            return Y>k*X+X0;
        }
        static bool point_on_straight(
                const Line& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            long double X0 = line.Y1() - k*line.X1();
            return std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
        }
        static bool straight_not_under_the_point(
                const Line& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            long double X0 = line.Y1() - k*line.X1();
            return Y<k*X+X0 || std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
        }
        static bool straight_not_above_the_point(
                const Line& line,
                long double X,
                long double Y) noexcept
        {
            long double k = line.k();
            if(std::isinf(k))
                return false;
            long double X0 = line.Y1() - k*line.X1();
            return Y>k*X+X0 || std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
        }
        bool intersects(const Line& other) const noexcept{
            return ::intersects(first(),second(),other.first(),other.second());
        }
        static Line cut(
                const Line& line,
                const Line& other,
                bool left_forward
                ) noexcept
        {
            auto point = line.intersection(other);
            if(!point.has_value())
                return line;
            Coord intersection_point{.lat_ = point.value().second,.lon_=point.value().first};
            if(!::on_segment(line.first(),line.second(),intersection_point))
                return line;
            long double side = cross(other.first(), other.second(), line.first());
            if(left_forward){
                if(side>0)
                    return Line(intersection_point,line.first());
                else
                    return Line(line.second(),intersection_point);
            }
            else{
                if(side>0)
                    return Line(line.second(),intersection_point);
                else
                    return Line(intersection_point,line.first());
            }
        }
        static std::optional<std::pair<double,double>> intersection(
                long double X11,long double Y11, long double X12, long double Y12,
                long double X21, long double Y21, long double X22, long double Y22) noexcept{
            long double scale = std::max(std::abs(X12 - X11), std::abs(Y12 - Y11))
                * std::max(std::abs(X22 - X21), std::abs(Y22 - Y21));
            long double denominator = (Y22 - Y21) * (X12 - X11) - (X22 - X21) * (Y12 - Y11);
            if (scale == 0.0 || std::abs(denominator) < 1e-12 * scale)
                return std::nullopt;
            long double u_a = ((X22 - X21) * (Y11 - Y21) - (Y22 - Y21) * (X11 - X21)) / denominator;
            return std::make_pair(X11+u_a*(X12-X11),Y11+u_a*(Y12 - Y11));
        }
        std::optional<std::pair<double,double>> intersection(
                long double X1, long double Y1, long double X2, long double Y2) const noexcept{
            return intersection(X1_,Y1_,X2_,Y2_,X1,Y1,X2,Y2);
        }
        std::optional<std::pair<double,double>> intersection(
                const Line& other) const noexcept{
            return intersection(other.X1_,other.Y1_,other.X2_,other.Y2_);
        }
    };

    template<typename POINT_T,typename OTHER_POINT_T,
        uint64_t DIMENSIONS,uint64_t OTHER_DIMENSIONS>
    bool equivalent() noexcept{
        bool is_equal(const BasicLine<OTHER_POINT_T& other) const noexcept{
            if(this!=&other){
                return std::abs(static_cast<long double>(X1_)-other.X1_)<
                        std::numeric_limits<double>::epsilon() &&
                std::abs(static_cast<long double>(X2_)-other.X2_)<
                        std::numeric_limits<double>::epsilon() &&
                std::abs(static_cast<long double>(Y1_)-other.Y1_)<
                        std::numeric_limits<double>::epsilon() &&
                std::abs(static_cast<long double>(Y2_)-other.Y2_)<
                        std::numeric_limits<double>::epsilon();
            }
            else return true;
        }
    }

    template<typename POINT_T, typename OTHER_POINT_T>
    std::vector<Intersection<2>> intersections(
        const BasicLine<POINT_T,2>& first,
        const BasicLine<OTHER_POINT_T,2>& second) noexcept
    {
        long double scale = std::max(std::abs(X12 - X11), std::abs(Y12 - Y11))
            * std::max(std::abs(X22 - X21), std::abs(Y22 - Y21));
        long double denominator = (Y22 - Y21) * (X12 - X11) - (X22 - X21) * (Y12 - Y11);
        if (scale == 0.0 || std::abs(denominator) < 1e-12 * scale)
            return std::nullopt;
        long double u_a = ((X22 - X21) * (Y11 - Y21) - (Y22 - Y21) * (X11 - X21)) / denominator;
        return std::make_pair(X11+u_a*(X12-X11),Y11+u_a*(Y12 - Y11));
    }
}