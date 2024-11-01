#pragma once
#include <vector>

int clamp(int x, int left, int right);
struct Color{
    int R = 0;
    int G = 0;
    int B = 0;

    bool operator<(const Color& other) const;

    bool operator>(const Color& other) const;

    Color operator+(const Color& other);

    Color operator-(const Color& other);

    Color operator*(double factor);

    Color operator/(double factor);
};

bool operator<(const Color& lhs, const Color& rhs);

bool operator>(const Color& lhs, const Color& rhs);

Color operator+(const Color& lhs, const Color& rhs);

Color operator-(const Color& lhs, const Color& rhs);

Color operator*(double factor,const Color& color);

Color operator/(double factor,const Color& color);

struct ColorAtValue{
    Color color;
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

Color get_color_grad(double value, std::vector<ColorAtValue> color_grad);