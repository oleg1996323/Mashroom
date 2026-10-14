#pragma once
#include <cmath>
#include <numbers>
#include <system_error>
#include <optional>
#include "algorithms.h"

class Line{
    double X1_;
    double Y1_;
    double X2_;
    double Y2_;
    public:
    using id = uint64_t;
    Line() = default;
    Line(double X1,double Y1,double X2,double Y2) noexcept:
    X1_(X1),Y1_(Y1),X2_(X2),Y2_(Y2){}
    Line(Coord first,Coord second) noexcept:
    X1_(first.lon_),Y1_(first.lat_),X2_(second.lon_),Y2_(second.lat_){}
    Line(const Line& other) = default;
    Line(Line&& other) noexcept = default;
    Line& operator=(const Line& other) = default;
    Line& operator=(Line&& other) noexcept = default;
    bool operator==(const Line&) = delete;
    bool is_equal(const Line& other) const noexcept{
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

template<>
struct std::hash<Line>{
    size_t operator()(const Line& val) const noexcept{
        return (std::hash<double>()(val.X1())<<3)^(std::hash<double>()(val.Y1())<<2)^
            (std::hash<double>()(val.X2())<<1)^std::hash<double>()(val.Y2());
    }
};

template<>
struct std::equal_to<Line>{
    bool operator()(const Line& lhs,const Line& rhs)const noexcept{
        if(&lhs!=&rhs){
            return  lhs.X1()==rhs.X1() &&
                    lhs.X2()==rhs.X2() &&
                    lhs.Y1()==rhs.Y1() &&
                    lhs.Y2()==rhs.Y2();
        }
        else return true;
    }
};