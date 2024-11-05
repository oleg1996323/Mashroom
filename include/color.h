#pragma once
#include <vector>
#include <type_traits>
#include <limits>
#include <limits.h>
#include <inttypes.h>

template<typename T1,typename T2, typename T3>
std::common_type_t<std::common_type_t<T1,T2>,T3> clamp(T1 x, T2 left, T3 right) {
    return (x < left ? left : (x > right ? right : x));
}

template<uint8_t T>
concept BitDepthCor = requires(uint8_t bd)
{
    bd==8 || bd == 16;
};

template<uint8_t BIT_DEPTH=8>
requires BitDepthCor<BIT_DEPTH>
struct RGB{
    int R = 0;
    int G = 0;
    int B = 0;
    uint8_t bit_depth=BIT_DEPTH;

    template<typename T>
    requires std::is_same_v<std::decay_t<T>,RGB<BIT_DEPTH>>
    inline bool operator<(T&& other) const{
        return R+G+B<other.R+other.G+other.B;
    }

    template<typename T>
    requires std::is_same_v<std::decay_t<T>,RGB<BIT_DEPTH>>
    inline bool operator>(T&& other) const{
        return R+G+B>other.R+other.G+other.B;
    }

    template<typename T>
    requires std::is_same_v<std::decay_t<T>,RGB<BIT_DEPTH>>
    inline RGB<BIT_DEPTH> operator+(T&& other) const{
        return {clamp(R+other.R,0,UCHAR_MAX),clamp(G+other.G,0,UCHAR_MAX),clamp(B+other.B,0,UCHAR_MAX)};
    }

    template<typename T>
    requires std::is_same_v<std::decay_t<T>,RGB<BIT_DEPTH>>
    inline RGB<BIT_DEPTH> operator-(T&& other) const{
        return {clamp(R-other.R,0,UCHAR_MAX),clamp(G-other.G,0,UCHAR_MAX),clamp(B-other.B,0,UCHAR_MAX)};
    }

    template<typename T>
    requires std::is_integral_v<T>
    inline RGB<BIT_DEPTH> operator*(T factor) const{
        return {(int)clamp(R*factor,0,UCHAR_MAX),(int)clamp(G*factor,0,UCHAR_MAX),(int)clamp(B*factor,0,UCHAR_MAX)};
    }

    template<typename T>
    requires std::is_integral_v<T>
    inline RGB<BIT_DEPTH> operator/(T factor) const{
        return {(int)clamp(R/factor,0,UCHAR_MAX),(int)clamp(G/factor,0,UCHAR_MAX),(int)clamp(B/factor,0,UCHAR_MAX)};
    }
};

template<uint8_t BIT_DEPTH>
bool operator<(const RGB<BIT_DEPTH>& lhs, const RGB<BIT_DEPTH>& rhs);

template<uint8_t BIT_DEPTH>
bool operator>(const RGB<BIT_DEPTH>& lhs, const RGB<BIT_DEPTH>& rhs);

template<uint8_t BIT_DEPTH>
RGB<BIT_DEPTH> operator+(const RGB<BIT_DEPTH>& lhs, const RGB<BIT_DEPTH>& rhs);

template<uint8_t BD_1, uint8_t BD_2>
RGB<BD_1==BD_2?BD_1:0> operator-(const RGB<BD_1>& lhs, const RGB<BD_2>& rhs);

template<uint8_t BIT_DEPTH>
RGB<BIT_DEPTH> operator*(const RGB<BIT_DEPTH>& color,double factor);

template<uint8_t BIT_DEPTH>
RGB<BIT_DEPTH> operator*(double factor,const RGB<BIT_DEPTH>& color);

template<uint8_t BIT_DEPTH>
RGB<BIT_DEPTH> operator/(const RGB<BIT_DEPTH>& color,double factor);

struct HSV{
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1

    bool operator<(const HSV& other) const;

    bool operator>(const HSV& other) const;

    HSV operator+(const HSV& other);

    HSV operator-(const HSV& other);

    HSV operator*(double factor);

    HSV operator/(double factor);
};

HSV rgb2hsv(RGB in);
RGB hsv2rgb(HSV in);

HSV rgb2hsv(RGB in);
RGB hsv2rgb(HSV in);

template<uint8_t BIT_DEPTH=8>
requires BitDepthCor<BIT_DEPTH>
struct ColorAtValue{
    RGB<BIT_DEPTH> color;
    double value;

    bool operator<(const ColorAtValue& other) const;

    bool operator<(double val) const;

    bool operator>(const ColorAtValue& other) const;

    bool operator>(double val) const;
};

bool operator<(const ColorAtValue& lhs, const ColorAtValue& rhs);

template<typename T>
requires std::is_same_v<std::decay_t<T>,ColorAtValue<BIT_DEPTH>>
bool operator>(const ColorAtValue& lhs, const ColorAtValue& rhs);

template<typename T>
requires std::is_same_v<std::decay_t<T>,ColorAtValue<BIT_DEPTH>>
bool operator<(double lhs, const ColorAtValue& rhs);

template<typename T>
requires std::is_same_v<std::decay_t<T>,ColorAtValue<BIT_DEPTH>>
bool operator>(double lhs, const ColorAtValue& rhs);

RGB get_color_grad(double value, std::vector<ColorAtValue> color_grad);