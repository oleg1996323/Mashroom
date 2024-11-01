#include "color.h"
#include <cassert>
#include <limits>
#include <limits.h>
#include <algorithm>

int clamp(int x, int left, int right) {
    return (x < left ? left : (x > right ? right : x));
}

Color get_color_grad(double value, std::vector<ColorAtValue> color_grad)
{
    if(color_grad.size()>1){
        // std::vector<ColorAtValue>::const_iterator lesser = std::lower_bound(color_grad.begin(),color_grad.end(),value/*,[](const ColorAtValue& lhs, const ColorAtValue& rhs){
        //     return
        // }*/);

        std::vector<ColorAtValue>::const_iterator lesser = std::find_if(color_grad.begin(),color_grad.end(),[&value](const ColorAtValue& col_val){
            return col_val.value>value;
        });
        if(lesser==color_grad.end()){
            return (color_grad.end()-1)->color;
        }
        else if(lesser==color_grad.begin()){
            return color_grad.begin()->color;
        }
        else{
            return (lesser-1)->color + 
            (value - (lesser-1)->value)/(lesser->value-(lesser-1)->value)*(lesser->color-(lesser-1)->color);
        }
    }
    return Color({0,0,0});
}

bool Color::operator<(const Color& other) const{
        return R+G+B<other.R+other.G+other.B;
    }

bool Color::operator>(const Color& other) const{
    return R+G+B>other.R+other.G+other.B;
}

Color Color::operator+(const Color& other){
    return {clamp(R+other.R,0,UCHAR_MAX),clamp(G+other.G,0,UCHAR_MAX),clamp(B+other.B,0,UCHAR_MAX)};
}

Color Color::operator-(const Color& other){
    return {clamp(R-other.R,0,UCHAR_MAX),clamp(G-other.G,0,UCHAR_MAX),clamp(B-other.B,0,UCHAR_MAX)};
}

Color Color::operator*(double factor){
    return {clamp(R*factor,0,UCHAR_MAX),clamp(G*factor,0,UCHAR_MAX),clamp(B*factor,0,UCHAR_MAX)};
}

Color Color::operator/(double factor){
    return {clamp(R/factor,0,UCHAR_MAX),clamp(G/factor,0,UCHAR_MAX),clamp(B/factor,0,UCHAR_MAX)};
}

bool operator<(const Color& lhs, const Color& rhs){
    return lhs.operator<(rhs);
}

bool operator>(const Color& lhs, const Color& rhs){
    return lhs.operator>(rhs);
}

Color operator+(const Color& lhs, const Color& rhs){
    return {clamp(lhs.R+rhs.R,0,UCHAR_MAX),
            clamp(lhs.G+rhs.G,0,UCHAR_MAX),
            clamp(lhs.B+rhs.B,0,UCHAR_MAX)};
}

Color operator-(const Color& lhs, const Color& rhs){
    return {clamp(lhs.R-rhs.R,0,UCHAR_MAX),
            clamp(lhs.G-rhs.G,0,UCHAR_MAX),
            clamp(lhs.B-rhs.B,0,UCHAR_MAX)};
}

Color operator*(double factor,const Color& color){
    return {clamp(color.R*factor,0,UCHAR_MAX),
            clamp(color.G*factor,0,UCHAR_MAX),
            clamp(color.B*factor,0,UCHAR_MAX)};
}

Color operator/(double factor,const Color& color){
    return {clamp(color.R/factor,0,UCHAR_MAX),
            clamp(color.G/factor,0,UCHAR_MAX),
            clamp(color.B/factor,0,UCHAR_MAX)};
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