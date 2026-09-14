#pragma once
#include <cmath>
#include <numbers>
#include <system_error>
#include <optional>

class Line{
    double X1_;
    double Y1_;
    double X2_;
    double Y2_;
    public:
    Line() = default;
    Line(double X1,double Y1,double X2,double Y2) noexcept:
    X1_(X1),Y1_(Y1),X2_(X2),Y2_(Y2){}
    Line(const Line& other) = default;
    Line(Line&& other) noexcept = default;
    Line& operator=(const Line& other) = default;
    Line& operator=(Line&& other) noexcept = default;
    bool operator==(const Line&) = delete;
    bool is_equal(const Line& other) const noexcept{
        if(this!=&other){
            return std::abs(X1_-other.X1_)<std::numeric_limits<double>::epsilon() &&
            std::abs(X2_-other.X2_)<std::numeric_limits<double>::epsilon() &&
            std::abs(Y1_-other.Y1_)<std::numeric_limits<double>::epsilon() &&
            std::abs(Y2_-other.Y2_)<std::numeric_limits<double>::epsilon();
        }
        else return true;
    }
    bool operator<(const Line& other) const {
        if (X1_ != other.X1_) return X1_ < other.X1_;
        if (Y1_ != other.Y1_) return Y1_ < other.Y1_;
        if (X2_ != other.X2_) return X2_ < other.X2_;
        return Y2_ < other.Y2_;
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
        double dX = X2_- X1_;
        double dY = Y2_- Y1_;
        return std::sqrt(dX*dX+dY*dY);
    }
    double k() const noexcept{
        return k(X1_,Y1_,X2_,Y2_);
    }
    static double k(double X1,double Y1,double X2, double Y2) noexcept{
        double dX = X2- X1;
        if(std::abs(dX)<std::numeric_limits<double>::epsilon()){
            return std::numeric_limits<double>::infinity();
        }
        double dY = Y2- Y1;
        return dY/dX;
    }
    double angle() const noexcept{
        return angle(X1_,Y1_,X2_,Y2_);
    }
    static double angle(double X1,double Y1,double X2, double Y2) noexcept{
        return atan(k(X1,Y1,X2,Y2));
    }
    bool is_vertical() const noexcept{
        double k_ = k();
        return std::isinf(k_);
    }
    bool is_horizontal() const noexcept{
        return std::abs(Y2_ - Y1_) < std::numeric_limits<double>::epsilon();
    }
    static bool straight_above_the_point(double X1,double Y1,double k, double X,double Y) noexcept{
        if(std::isinf(k))
            return false;
        double X0 = Y1 - k*X1;
        return Y>k*X+X0;
    }
    static bool straight_under_the_point(double X1,double Y1,double k, double X,double Y) noexcept{
        if(std::isinf(k))
            return false;
        double X0 = Y1 - k*X1;
        return Y<k*X+X0;
    }
    static bool point_on_straight(double X1,double Y1,double k, double X,double Y) noexcept{
        if(std::isinf(k))
            return false;
        double X0 = Y1 - k*X1;
        return std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
    }
    static bool straight_not_under_the_point(double X1,double Y1,double k, double X,double Y) noexcept{
        if(std::isinf(k))
            return false;
        double X0 = Y1 - k*X1;
        return Y<k*X+X0 || std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
    }
    static bool straight_not_above_the_point(double X1,double Y1,double k, double X,double Y) noexcept{
        if(std::isinf(k))
            return false;
        double X0 = Y1 - k*X1;
        return Y>k*X+X0 || std::abs(Y-k*X+X0)<std::numeric_limits<double>::epsilon();
    }
    static std::optional<std::pair<double,double>> intersection(
            double X11,double Y11,double X12,double Y12,
            double X21,double Y21,double X22,double Y22) noexcept{
        double denominator = (Y22 - Y21) * (X12 - X11) - (X22 - X21) * (Y12 - Y11);
        if(denominator==0)
            return std::nullopt;
        double u_a = ((X22 - X21) * (Y11 - Y21) - (Y22 - Y21) * (X11 - X21)) / denominator;
        return std::make_pair(X11+u_a*(X12-X11),Y11+u_a*(Y12 - Y11));
    }
    std::optional<std::pair<double,double>> intersection(
            double X1,double Y1,double X2,double Y2) const noexcept{
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