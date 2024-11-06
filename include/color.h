#pragma once
#include <vector>
#include <type_traits>
#include <limits>
#include <limits.h>
#include <inttypes.h>
#include "interp_pp.h"

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
    requires std::is_same_v<std::decay_t<T>,RGB<BIT_DEPTH>>
    inline bool operator==(T&& other) const{
        return R == other.R && G ==other.G && B == other.B;
    }

    template<typename T>
    requires std::is_same_v<std::decay_t<T>,RGB<BIT_DEPTH>>
    inline bool operator==(T&& other){
        return R == other.R && G ==other.G && B == other.B;
    }

    template<typename T>
    requires std::is_same_v<std::decay_t<T>,RGB<BIT_DEPTH>>
    inline bool operator!=(T&& other) const{
        return !(*this==other);
    }

    template<typename T>
    requires std::is_fundamental_v<T>
    inline RGB<BIT_DEPTH> operator*(T factor) const{
        return {(int)clamp(R*factor,0,UCHAR_MAX),(int)clamp(G*factor,0,UCHAR_MAX),(int)clamp(B*factor,0,UCHAR_MAX)};
    }

    template<typename T>
    requires std::is_fundamental_v<T>
    inline RGB<BIT_DEPTH> operator/(T factor) const{
        return {(int)clamp(R/factor,0,UCHAR_MAX),(int)clamp(G/factor,0,UCHAR_MAX),(int)clamp(B/factor,0,UCHAR_MAX)};
    }
};

template<uint8_t BIT_DEPTH=8>
bool operator<(const RGB<BIT_DEPTH>& lhs, const RGB<BIT_DEPTH>& rhs){
    return lhs.operator<(rhs);
}

template<uint8_t BIT_DEPTH=8>
bool operator>(const RGB<BIT_DEPTH>& lhs, const RGB<BIT_DEPTH>& rhs){
    return lhs.operator>(rhs);
}

template<typename COL_T, typename T>
requires std::is_fundamental_v<T>
std::decay_t<COL_T> operator*(COL_T&& color,T factor){
    return color*factor;
}

template<typename COL_T, typename T>
requires std::is_fundamental_v<T>
std::decay_t<COL_T> operator/(COL_T&& color,T factor){
    return color/factor;
}

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

HSV rgb2hsv(RGB<> in);
RGB<> hsv2rgb(HSV in);

HSV rgb2hsv(RGB<> in);
RGB<> hsv2rgb(HSV in);

template<typename T>
concept ColorOperation = requires(T color_1, T color_2)
{
    std::default_initializable<T>;
    std::is_default_constructible_v<T>;
    color_1<color_2;
    color_1>color_2;
    color_1==color_2;
    color_1!=color_2;
    color_1+color_2;
    color_1-color_2;
};

template<typename COL_T>
requires ColorOperation<COL_T>
struct ColorAtValue{
    std::decay_t<COL_T> color;
    double value;

    inline bool operator<(const ColorAtValue<COL_T>& other) const{
        return value<other.value;
    }

    inline bool operator>(const ColorAtValue<COL_T>& other) const {
        return value>other.value;
    }

    inline bool operator<=(const ColorAtValue<COL_T>& other) const{
        return value<=other.value;
    }

    inline bool operator>=(const ColorAtValue<COL_T>& other) const {
        return value>=other.value;
    }

    inline bool operator==(const ColorAtValue<COL_T>& val) const {
        return value==val;
    }

    inline bool operator!=(const ColorAtValue<COL_T>& val) const {
        return !(value==val);
    }
};

template<typename COL_T>
bool operator<(ColorAtValue<COL_T>&& lhs, ColorAtValue<COL_T>&& rhs){
    return lhs.value<rhs.value;
}

template<typename COL_T>
bool operator>(ColorAtValue<COL_T>&& lhs, ColorAtValue<COL_T>&& rhs){
    return lhs.value>rhs.value;
}

template<typename COL_T>
bool operator==(ColorAtValue<COL_T>&& lhs, ColorAtValue<COL_T>&& rhs){
    return lhs.value==rhs.value;
}

template<typename COL_T, typename T>
//requires std::is_default_constructible_v<COL_T> && std::is_fundamental_v<std::decay_t<T>>
COL_T get_color_grad(T&& value, std::vector<ColorAtValue<COL_T>> color_grad){
    if(color_grad.size()>1){
        typename std::vector<ColorAtValue<COL_T>>::const_iterator lesser = std::find_if(color_grad.begin(),color_grad.end(),[&value](const ColorAtValue<COL_T>& col_val){
            return col_val.value>=value;
        });
        if(lesser==color_grad.end()){
            return (color_grad.end()-1)->color;
        }
        else if(lesser==color_grad.begin()){
            return color_grad.begin()->color;
        }
        else{
            // double koef = (value - (lesser-1)->value)/(lesser->value - (lesser-1)->value);
            // ColorAtValue y1 = *(lesser-1);
            // RGB y22 = koef*lesser->color;
            // RGB y21 = koef*(lesser-1)->color;
            // RGB dif = y22 - y21;
            //return (lesser-1)->color+koef*lesser->color-koef*(lesser-1)->color;
            //return hsv2rgb(lin_interp_between(rgb2hsv((lesser-1)->color),(lesser-1)->value,rgb2hsv(lesser->color),lesser->value,value));
            return lin_interp_between((lesser-1)->color,(lesser-1)->value,lesser->color,lesser->value,value);
        }
    }
    return COL_T();
}