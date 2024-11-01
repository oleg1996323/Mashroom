#pragma once
#ifdef __cplusplus
extern "C"{
    #include "png.h"
    #include "def.h"
    #undef max
    #undef min
}
#endif
#include <string>
#include <vector>
#include "color.h"

void PngOut(std::string name,ValueByCoord* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad);

void PngOut(std::string name,float* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad);

void PngOutGray(std::string name,float* values, png_uint_32 w,png_uint_32 h);

void PngOutRGBGradient(std::string name,float* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad, bool minmax_grad);