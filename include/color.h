#pragma once
#include <vector>

int clamp(int x, int left, int right);

struct RGB{
    int R = 0;
    int G = 0;
    int B = 0;

    bool operator<(const RGB& other) const;

    bool operator>(const RGB& other) const;

    RGB operator+(const RGB& other);

    RGB operator-(const RGB& other);

    RGB operator*(double factor);

    RGB operator/(double factor);
};

bool operator<(const RGB& lhs, const RGB& rhs);

bool operator>(const RGB& lhs, const RGB& rhs);

RGB operator+(const RGB& lhs, const RGB& rhs);

RGB operator-(const RGB& lhs, const RGB& rhs);

RGB operator*(double factor,const RGB& color);

RGB operator/(double factor,const RGB& color);

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

HSV   rgb2hsv(RGB in);
RGB   hsv2rgb(HSV in);

HSV rgb2hsv(RGB in);
RGB hsv2rgb(HSV in);
struct ColorAtValue{
    RGB color;
    double value;

    bool operator<(const ColorAtValue& other) const;

    bool operator<(double val) const;

    bool operator>(const ColorAtValue& other) const;

    bool operator>(double val) const;
};

bool operator<(const ColorAtValue& lhs, const ColorAtValue& rhs);

bool operator>(const ColorAtValue& lhs, const ColorAtValue& rhs);

bool operator<(double lhs, const ColorAtValue& rhs);

bool operator>(double lhs, const ColorAtValue& rhs);

RGB get_color_grad(double value, std::vector<ColorAtValue> color_grad);