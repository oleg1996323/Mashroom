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
#include <iterator>
#include <algorithm>

// void PngOut(std::string name,ValueByCoord* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad);

// void PngOut(std::string name,float* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad);

void PngOutGray(std::string name, const float* values, png_uint_32 w,png_uint_32 h);

template<typename COL_T>
requires std::is_class_v<COL_T>
void PngOutRGBGradient(std::string name, const float* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue<COL_T>> color_grad, bool minmax_grad){
    if(minmax_grad){
        std::pair<std::vector<float>::iterator, std::vector<float>::iterator> minmax;
        {
            std::vector<float> data(values,values+w*h);
            minmax = std::minmax_element(data.begin(),data.end());
        }
        if(!color_grad.empty()){

            std::sort(color_grad.begin(),color_grad.end());
            float diff = (*minmax.second - *minmax.first)/(color_grad.size()-1);
            for(int i = 0;i<color_grad.size();++i){
                color_grad.at(i).value = (*minmax.first)+diff*i;
            }
        }
        else {
            color_grad.push_back({{0,0,UCHAR_MAX},*minmax.first});
            color_grad.push_back({{UCHAR_MAX,0,0},*minmax.second});
        }
    }
    uint8_t bit_depth =16;
    
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
    
    png_set_IHDR(png_ptr, png_info, w, h, bit_depth, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    unsigned short* data_ptr = (unsigned short*)malloc(w*h*3*sizeof(unsigned short));
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            size_t iter = (i*w+j)*3;
            auto c = get_color_grad(values[j+w*i],color_grad);
            data_ptr[iter] = (unsigned short)c.R;
            data_ptr[iter+1] = (unsigned short)c.G;
            data_ptr[iter+2] = (unsigned short)c.B;
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