#include "images/png_out.h"
#include <algorithm>
#include <iostream>
#include "interpolation.h"

void PngOutGray(std::string name,const float* values, const Size& sz){
    uint32_t w = sz.width();
    uint32_t h = sz.height();
    std::vector<float> data(values,values+w*h);
    auto minmax = std::minmax_element(data.begin(),data.end());
    uint8_t bit_depth = 16;
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
    
    png_set_IHDR(png_ptr, png_info, w, h, bit_depth, PNG_COLOR_TYPE_GRAY,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);
    png_bytepp data_png;

    unsigned short* data_ptr = (unsigned short*)malloc(sizeof(unsigned short)*w*h);
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            data_ptr[i*w+j] = lin_interp_between(0,*minmax.first,SHRT_MAX,*minmax.second,values[j+w*i]);
        }
    }

    png_write_info (png_ptr, png_info);
    png_set_swap(png_ptr);

    for (int i=0; i<h; i++)
        png_write_row (png_ptr, (png_const_bytep)&data_ptr[i*w]);
    png_write_end (png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    //free(data);
}
