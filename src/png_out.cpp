#include "png_out.h"
#include <algorithm>
#include <iostream>
#ifdef __cplusplus
extern "C"{
    #include "interpolation.h"
}
#endif

void PngOutGray(std::string name,float* values, png_uint_32 w,png_uint_32 h){
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
    int sz  =sizeof(unsigned short)*w*h;
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


    // int count = 0;
    // for (int i = 0; i < h; ++i) {
    //     rows[h - i - 1] = data_ptr + (i*w);
    //     for (int j = 0; j < w; ++j) {
    //         int i1 = (i*w+j);
    //         //std::cout<<"R: "<<(int)c.R<<" G: "<<(int)c.G<<" B: "<<(int)c.B<<std::endl;
    //         ++count;
    //         data_ptr[i1++] = values[j+w*i]/(*minmax.second-*minmax.first)/2*255;
    //     }
    // }
    // std::cout << "Count: "<<count<<std::endl;
    // png_set_rows(png_ptr, png_info, rows);
    // png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, nullptr);

    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    //free(data);
}

void PngOutRGBGradient(std::string name,float* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad={}, bool minmax_grad=true){
    std::pair<std::vector<float>::iterator, std::vector<float>::iterator> minmax;
    {
        std::vector<float> data(values,values+w*h);
        minmax = std::minmax_element(data.begin(),data.end());
    }
    if(minmax_grad){
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
    uint8_t bit_depth = 8;
    
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
    
    png_set_IHDR(png_ptr, png_info, w, h, 8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);
    png_bytepp data_png;

    unsigned char* data_ptr = (unsigned char*)malloc(w*h*3);
    unsigned char *rows[h];

    int count = 0;
    for (int i = 0; i < h; ++i) {
        rows[h - i - 1] = data_ptr + (i*w*3);
        for (int j = 0; j < w; ++j) {
            int i1 = (i*w+j)*3;
            RGB c = get_color_grad(values[j+w*i],color_grad);
            data_ptr[i1+=2] = c.R;
            data_ptr[i1+=2] = c.G;
            data_ptr[i1+=2] = c.B;
        }
    }

    png_set_rows(png_ptr, png_info, rows);
    png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, nullptr);
    png_write_end(png_ptr, png_info);

    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    free(data_ptr);
}