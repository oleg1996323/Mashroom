#include "color.h"
#include <cassert>
#include <limits>
#include <limits.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cmath>
#include "interpolation.h"


inline int clamp(int x, int left, int right) {
    return (x < left ? left : (x > right ? right : x));
}

template<typename 
double clamp(double x, double left, double right) {
    return (x < left ? left : (x > right ? right : x));
}

template<typename T_Y, typename T_X>
T_Y lin_interp_between(T_Y y1, T_X x1, T_Y y2, T_X x2, T_X x_value){
    if(x_value!=x1)
        return y1+(y2-y1)/(x2 - x1)*
            (x_value - x1);
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
            if(value<color_grad.at(color_grad.size()-3))
                std::cout<<value<<std::endl;
            double koef = (value - (lesser-1)->value)/(lesser->value - (lesser-1)->value);
            ColorAtValue y1 = *(lesser-1);
            RGB y22 = koef*lesser->color;
            RGB y21 = koef*(lesser-1)->color;
            RGB dif = y22 - y21;
            return (lesser-1)->color+koef*lesser->color-koef*(lesser-1)->color;
            return hsv2rgb(lin_interp_between(rgb2hsv((lesser-1)->color),(lesser-1)->value,rgb2hsv(lesser->color),lesser->value,value));
        }
    }
    return RGB({0,0,0});
}

bool RGB::operator<(const RGB& other) const{
        return R+G+B<other.R+other.G+other.B;
    }

bool RGB::operator>(const RGB& other) const{
    return R+G+B>other.R+other.G+other.B;
}

RGB RGB::operator+(const RGB& other){
    return {clamp(R+other.R,0,UCHAR_MAX),clamp(G+other.G,0,UCHAR_MAX),clamp(B+other.B,0,UCHAR_MAX)};
}

RGB RGB::operator-(const RGB& other){
    return {clamp(R-other.R,0,UCHAR_MAX),clamp(G-other.G,0,UCHAR_MAX),clamp(B-other.B,0,UCHAR_MAX)};
}

RGB RGB::operator*(double factor){
    return {clamp(R*factor,0,UCHAR_MAX),clamp(G*factor,0,UCHAR_MAX),clamp(B*factor,0,UCHAR_MAX)};
}

RGB RGB::operator/(double factor){
    return {clamp(R/factor,0,UCHAR_MAX),clamp(G/factor,0,UCHAR_MAX),clamp(B/factor,0,UCHAR_MAX)};
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

RGB operator*(double factor,const RGB& color){
    return {clamp(color.R*factor,0,UCHAR_MAX),
            clamp(color.G*factor,0,UCHAR_MAX),
            clamp(color.B*factor,0,UCHAR_MAX)};
}

RGB operator/(double factor,const RGB& color){
    return {clamp(color.R/factor,0,UCHAR_MAX),
            clamp(color.G/factor,0,UCHAR_MAX),
            clamp(color.B/factor,0,UCHAR_MAX)};
}

bool HSV::operator<(const HSV& other) const{
    return v<other.v;
}

bool HSV::operator>(const HSV& other) const{
    return v>other.v;
}

HSV HSV::operator+(const HSV& other){
    return {std::fmod(h+other.h,360),clamp(h+other.h,0,1),std::fmod(h+other.h,1)};
}

HSV HSV::operator-(const HSV& other){

}

HSV HSV::operator*(double factor){

}

HSV HSV::operator/(double factor){

}

bool ColorAtValue::operator<(const ColorAtValue& other) const {
    return value<other.value;
}

bool ColorAtValue::operator<(double val) const {
    return value<val;
}

bool ColorAtValue::operator>(const ColorAtValue& other) const {
    return value>other.value;
}

bool ColorAtValue::operator>(double val) const {
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