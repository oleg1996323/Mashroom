#pragma once
#include <vector>
#include <type_traits>
#include <limits>
#include <limits.h>
#include <inttypes.h>
#include "interp_pp.h"

template<uint8_t BIT_DEPTH>
inline constexpr uint16_t max_value_bit_depth(){return BIT_DEPTH==8?UCHAR_MAX:USHRT_MAX;}

template<typename T1,typename T2, typename T3, typename Tr = T1>
inline Tr clamp(T1 x, T2 left, T3 right) {
    return (x < left ? left : (x > right ? right : x));
}

template<uint8_t T>
concept BitDepthCor = (T == 8 || T == 16);

template<typename COL_T, uint8_t BIT_DEPTH>
concept CheckColorStruct = requires (COL_T color){
    { COL_T::bit_depth } -> std::convertible_to<uint8_t>;
    requires COL_T::bit_depth == BIT_DEPTH;
};

template<uint8_t BIT_DEPTH=8>
requires BitDepthCor<BIT_DEPTH>
struct RGB{
    int R = 0;
    int G = 0;
    int B = 0;
    static constexpr uint8_t bit_depth=BIT_DEPTH;
    
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
        return RGB<BIT_DEPTH>({clamp(R+other.R,0,max_value_bit_depth<BIT_DEPTH>()),
                clamp(G+other.G,0,max_value_bit_depth<BIT_DEPTH>()),
                clamp(B+other.B,0,max_value_bit_depth<BIT_DEPTH>())});
    }

    template<typename T>
    requires std::is_same_v<std::decay_t<T>,RGB<BIT_DEPTH>>
    inline RGB<BIT_DEPTH> operator-(T&& other) const{
        return RGB<BIT_DEPTH>({clamp(R-other.R,0,max_value_bit_depth<BIT_DEPTH>()),
                                clamp(G-other.G,0,max_value_bit_depth<BIT_DEPTH>()),
                                clamp(B-other.B,0,max_value_bit_depth<BIT_DEPTH>())});
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
    requires std::is_fundamental_v<std::decay_t<T>>
    inline RGB<BIT_DEPTH> operator*(std::decay_t<T> factor) const{
        return RGB<BIT_DEPTH>({(int)clamp(R*factor,0,max_value_bit_depth<BIT_DEPTH>()),
                                (int)clamp(G*factor,0,max_value_bit_depth<BIT_DEPTH>()),
                                (int)clamp(B*factor,0,max_value_bit_depth<BIT_DEPTH>())});
    }

    template<typename T>
    requires std::is_fundamental_v<std::decay_t<T>>
    inline RGB<BIT_DEPTH> operator/(std::decay_t<T> factor) const{
        return RGB<BIT_DEPTH>({(int)clamp(R/factor,0,max_value_bit_depth<BIT_DEPTH>()),
                                (int)clamp(G/factor,0,max_value_bit_depth<BIT_DEPTH>()),
                                (int)clamp(B/factor,0,max_value_bit_depth<BIT_DEPTH>())});
    }
};

template<uint8_t BIT_DEPTH=8>
inline bool operator<(const RGB<BIT_DEPTH>& lhs, const RGB<BIT_DEPTH>& rhs){
    return lhs.operator<(rhs);
}

template<uint8_t BIT_DEPTH=8>
inline bool operator>(const RGB<BIT_DEPTH>& lhs, const RGB<BIT_DEPTH>& rhs){
    return lhs.operator>(rhs);
}

template<uint8_t BIT_DEPTH, typename T>
requires std::is_fundamental_v<std::decay_t<T>>
inline std::decay_t<RGB<BIT_DEPTH>> operator*(T&& factor,const RGB<BIT_DEPTH>& color){
    return RGB<BIT_DEPTH>({(int)clamp(color.R*factor,0,max_value_bit_depth<BIT_DEPTH>()),
                        (int)clamp(color.G*factor,0,max_value_bit_depth<BIT_DEPTH>()),
                        (int)clamp(color.B*factor,0,max_value_bit_depth<BIT_DEPTH>())});
}

template<uint8_t BIT_DEPTH, typename T>
requires std::is_fundamental_v<std::decay_t<T>>
inline std::decay_t<RGB<BIT_DEPTH>> operator/(T&& factor,const RGB<BIT_DEPTH>& color){
    return RGB<BIT_DEPTH>({(int)clamp(color.R/factor,0,max_value_bit_depth<BIT_DEPTH>()),
                        (int)clamp(color.G/factor,0,max_value_bit_depth<BIT_DEPTH>()),
                        (int)clamp(color.B/factor,0,max_value_bit_depth<BIT_DEPTH>())});
}

template<uint8_t BIT_DEPTH, typename T_X1, typename T_X2, typename T_X_VAL>
RGB<BIT_DEPTH> lin_interp_between(const RGB<BIT_DEPTH>& y_1, T_X1&& x_1, const RGB<BIT_DEPTH>& y_2, T_X2&& x_2, T_X_VAL&& x_value){
    if(x_value==x_2)
        return y_2;
    else if(x_value==x_1)
        return y_1;
    else{
        int R = (y_2.R - y_1.R)*(x_value - x_1)/(x_2 - x_1)+y_1.R;
        int G = (y_2.G - y_1.G)*(x_value - x_1)/(x_2 - x_1)+y_1.G;
        int B = (y_2.B - y_1.B)*(x_value - x_1)/(x_2 - x_1)+y_1.B;
        return {R<0?0:R,G<0?0:G,B<0?0:B};
    }
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
            return lin_interp_between((lesser-1)->color,(lesser-1)->value,lesser->color,lesser->value,value);
        }
    }
    return COL_T();
}