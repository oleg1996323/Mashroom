#pragma once
#ifdef __cplusplus
extern "C"{
    #include "png.h"
}
#endif
#include "def.h"
#undef max
#undef min
#include <string>
#include <vector>
#include "color.h"
#include <iterator>
#include <algorithm>
#include "types/size.h"
#include <iostream>

// void PngOut(std::string name,ValueByCoord* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad);

// void PngOut(std::string name,float* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad);

void PngOutGray(std::string name, const float* values, const Size& sz);

template<uint8_t BIT_DEPTH = 8,typename COL_T> //add checking BIT_DEPTH by concept
requires std::is_class_v<COL_T> && BitDepthCor<BIT_DEPTH> && CheckColorStruct<COL_T, BIT_DEPTH>
void PngOutRGBGradient(std::string name, const float* values, const Size& sz, std::vector<ColorAtValue<COL_T>> color_grad, bool minmax_grad){
    using DATA_TYPE = typename std::conditional<(BIT_DEPTH ==8), uint8_t, uint16_t>::type;

    uint32_t w = sz.width();
    uint32_t h = sz.height();
    if(minmax_grad){
        float max;
        float min;
        {
            std::vector<float> data(values,values+w*h);
            auto minmax = std::minmax_element(data.begin(),data.end());
            max = *minmax.second;
            min = *minmax.first;
        }
        if(!color_grad.empty()){

            std::sort(color_grad.begin(),color_grad.end());
            float diff = (max - min)/(color_grad.size()-1);
            for(int i = 0;i<color_grad.size();++i){
                color_grad.at(i).value = (min)+diff*i;
            }
        }
        else {
            color_grad.push_back({{0,0,max_value_bit_depth<BIT_DEPTH>()},min});
            color_grad.push_back({{max_value_bit_depth<BIT_DEPTH>(),0,0},max});
        }
    }
    
    FILE *fp = fopen(name.c_str(), "wb");
    if (!fp) {
       return;
    }
    png_structp png_ptr = nullptr;
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
    }

    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return;
    }

    png_init_io(png_ptr, fp);
    
    png_set_IHDR(png_ptr, png_info, w, h, BIT_DEPTH, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    DATA_TYPE* data_ptr = (DATA_TYPE*)malloc(w*h*3*sizeof(DATA_TYPE));
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            size_t iter = (i*w+j)*3;
            auto c = get_color_grad(values[j+w*i],color_grad);
            data_ptr[iter++] = (DATA_TYPE)c.R;
            data_ptr[iter++] = (DATA_TYPE)c.G;
            data_ptr[iter++] = (DATA_TYPE)c.B;
        }
    }

    png_write_info (png_ptr, png_info);
    png_set_swap(png_ptr);

    for (int i=0; i<h; i++)
        png_write_row (png_ptr, (png_const_bytep)&data_ptr[i*w*3]);
    png_write_end (png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    free(data_ptr);
}