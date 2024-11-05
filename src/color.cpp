#include "color.h"
#include <cassert>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cmath>
#include "interpolation.h"

template<typename T_Y, typename T_X>
std::decay_t<T_Y> lin_interp_between(T_Y y1, T_X x1, T_Y y2, T_X x2, T_X x_value){
    if(x_value!=x1){
        return y1+(y2-y1)*((x_value - x1)/(x2 - x1));
    }
    else return y1;
}

RGB get_color_grad(double value, std::vector<ColorAtValue> color_grad)
{
    if(color_grad.size()>1){
        std::vector<ColorAtValue>::const_iterator lesser = std::find_if(color_grad.begin(),color_grad.end(),[&value](const ColorAtValue& col_val){
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
            return lin_interp_between<const RGB&, float>((lesser-1)->color,(lesser-1)->value,lesser->color,lesser->value,value);
        }
    }
    return RGB({0,0,0});
}

bool operator<(const RGB& lhs, const RGB& rhs){
    return lhs.operator<(rhs);
}

bool operator>(const RGB& lhs, const RGB& rhs){
    return lhs.operator>(rhs);
}

RGB operator+(const RGB& lhs, const RGB& rhs){
    return {clamp(lhs.R+rhs.R,0,UCHAR_MAX),
            clamp(lhs.G+rhs.G,0,UCHAR_MAX),
            clamp(lhs.B+rhs.B,0,UCHAR_MAX)};
}

RGB operator-(const RGB& lhs, const RGB& rhs){
    return {clamp(lhs.R-rhs.R,0,UCHAR_MAX),
            clamp(lhs.G-rhs.G,0,UCHAR_MAX),
            clamp(lhs.B-rhs.B,0,UCHAR_MAX)};
}

RGB operator*(const RGB& color,double factor){
    return {(int)clamp(color.R*factor,0,UCHAR_MAX),
            (int)clamp(color.G*factor,0,UCHAR_MAX),
            (int)clamp(color.B*factor,0,UCHAR_MAX)};
}

RGB operator*(double factor,const RGB& color){
    return {(int)clamp(color.R*factor,0,UCHAR_MAX),
            (int)clamp(color.G*factor,0,UCHAR_MAX),
            (int)clamp(color.B*factor,0,UCHAR_MAX)};
}

RGB operator/(const RGB& color,double factor){
    return {(int)clamp(color.R/factor,0,UCHAR_MAX),
            (int)clamp(color.G/factor,0,UCHAR_MAX),
            (int)clamp(color.B/factor,0,UCHAR_MAX)};
}

bool HSV::operator<(const HSV& other) const{
    return v<other.v;
}

bool HSV::operator>(const HSV& other) const{
    return v>other.v;
}

inline HSV HSV::operator+(const HSV& other){
    return {std::fmod(h+other.h,360),std::fmod(s+other.s,1),std::fmod(v+other.v,1)};
}

inline HSV HSV::operator-(const HSV& other){
    return {std::fmod(h-other.h,360),std::fmod(s-other.s,1),std::fmod(v-other.v,1)};
}

inline HSV HSV::operator*(double factor){
    return {std::fmod(h*factor,360),std::fmod(s*factor,1),std::fmod(v*factor,1)};
}

inline HSV HSV::operator/(double factor){
    return {std::fmod(h/factor,360),std::fmod(s/factor,1),std::fmod(v/factor,1)};
}

inline bool ColorAtValue::operator<(const ColorAtValue& other) const {
    return value<other.value;
}

inline bool ColorAtValue::operator<(double val) const {
    return value<val;
}

inline bool ColorAtValue::operator>(const ColorAtValue& other) const {
    return value>other.value;
}

inline bool ColorAtValue::operator>(double val) const {
    return value>val;
}

bool operator<(const ColorAtValue& lhs, const ColorAtValue& rhs){
    return lhs.value<rhs.value;
}

bool operator>(const ColorAtValue& lhs, const ColorAtValue& rhs){
    return lhs.value>rhs.value;
}

bool operator<(double lhs, const ColorAtValue& rhs){
    return lhs<rhs.value;
}

bool operator>(double lhs, const ColorAtValue& rhs){
    return lhs>rhs.value;
}

HSV rgb2hsv(RGB in)
{
    HSV         out;
    double      min, max, delta;
    min = in.R < in.G ? in.R : in.G;
    min = min  < in.B ? min  : in.B;
    max = in.R > in.G ? in.R : in.G;
    max = max  > in.B ? max  : in.B;
    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = -99999;                            // its now undefined
        return out;
    }
    if( in.R >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.G - in.B ) / delta;        // between yellow & magenta
    else
    if( in.G >= max )
        out.h = 2.0 + ( in.B - in.R ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.R - in.G ) / delta;  // between magenta & cyan
    out.h *= 60.0;                              // degrees
    if( out.h < 0.0 )
        out.h += 360.0;
    return out;
}

RGB hsv2rgb(HSV in)
{
    double      hh, p, q, t, ff;
    long        i;
    RGB         out;
    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.R = in.v;
        out.G = in.v;
        out.B = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));
    switch(i) {
    case 0:
        out.R = in.v;
        out.G = t;
        out.B = p;
        break;
    case 1:
        out.R = q;
        out.G = in.v;
        out.B = p;
        break;
    case 2:
        out.R = p;
        out.G = in.v;
        out.B = t;
        break;
    case 3:
        out.R = p;
        out.G = q;
        out.B = in.v;
        break;
    case 4:
        out.R = t;
        out.G = p;
        out.B = in.v;
        break;
    case 5:
    default:
        out.R = in.v;
        out.G = p;
        out.B = q;
        break;
    }
    return out;
}