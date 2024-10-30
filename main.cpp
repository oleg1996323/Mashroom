#include <iostream>
#include <dlfcn.h>
#include <memory>
#include <filesystem>
#include "functions.h"
#include <vector>
#include <algorithm>
#include <numeric>

#ifdef __cplusplus
extern "C"{
    #include <decode.h>
    #include <capitalize.h>
    #include <extract.h>
    #include <libpng/png.h>
}
#endif

struct Color{
    uint8_t R = 0;
    uint8_t G = 0;
    uint8_t B = 0;

    bool operator<(const Color& other) const{
        return R+G+B<other.R+other.G+other.B;
    }

    Color operator+(const Color& other){
        return {(uint8_t)(R+other.R),(uint8_t)(G+other.G),(uint8_t)(B+other.B)};
    }

    Color operator-(const Color& other){
        return {(uint8_t)(R-other.R),(uint8_t)(G-other.G),(uint8_t)(B-other.B)};
    }

    Color operator*(double factor){
        return {(uint8_t)(R*factor),(uint8_t)(G*factor),(uint8_t)(B*factor)};
    }

    Color operator/(double factor){
        return {(uint8_t)(R/factor),(uint8_t)(G/factor),(uint8_t)(B/factor)};
    }
};

bool operator<(const Color& lhs, const Color& rhs){
    return lhs.R+lhs.G+lhs.B<rhs.R+rhs.G+rhs.B;
}

Color operator+(const Color& lhs, const Color& rhs){
    return {(uint8_t)(lhs.R+rhs.R),(uint8_t)(lhs.G+rhs.G),(uint8_t)(lhs.B+rhs.B)};
}

Color operator-(const Color& lhs, const Color& rhs){
    return {(uint8_t)(lhs.R-rhs.R),(uint8_t)(lhs.G-rhs.G),(uint8_t)(lhs.B-rhs.B)};
}

Color operator*(const Color& color,double factor){
    return {(uint8_t)(color.R*factor),(uint8_t)(color.G*factor),(uint8_t)(color.B*factor)};
}

Color operator/(const Color& color,double factor){
    return {(uint8_t)(color.R/factor),(uint8_t)(color.G/factor),(uint8_t)(color.B/factor)};
}

struct ColorAtValue{
    Color color;
    double value;

    bool operator<(const ColorAtValue& other) const {
        return value<other.value;
    }

    bool operator<(double val) const {
        return value<val;
    }
};

bool operator<(const ColorAtValue& lhs, const ColorAtValue& rhs){
    return lhs.value<rhs.value;
}

Color get_color_grad(double value, std::vector<ColorAtValue> color_grad){
    if(color_grad.size()>1){
        std::vector<ColorAtValue>::const_iterator lesser = std::lower_bound(color_grad.begin(),color_grad.end(),value/*,[](const ColorAtValue& lhs, const ColorAtValue& rhs){
            return
        }*/);
        if(lesser==color_grad.cend()){
            return lesser->color;
        }
        else if(lesser==color_grad.cend()-1){
            return lesser->color;
        }
        else{
            return lesser->color + 
            ((lesser+1)->color-lesser->color)/((lesser+1)->value - lesser->value)*
            (value - lesser->value);
        }
    }
    return Color({0,0,0});
}

struct mem_encode
{
  char *buffer;
  size_t size;
};

void PngOut(std::string name,ValueByCoord* values, png_uint_32 w,png_uint_32 h, std::vector<ColorAtValue> color_grad={}){
    std::sort(color_grad.begin(),color_grad.end());
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

    unsigned char data[w*h*3];
    unsigned char *rows[h];

    for (int i = 0; i < h; ++i) {
        rows[h - i - 1] = data + (i*w*3);
        for (int j = 0; j < w; ++j) {
            int i1 = (i*w+j)*3;
            int i2 = (i*w+j)*4;
            Color c = get_color_grad(values[j+w*i].value,color_grad);
            data[i1++] = c.R;
            data[i1++] = c.G;
            data[i1++] = c.B;
        }
    }

    png_set_rows(png_ptr, png_info, rows);
    png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, nullptr);
    png_write_end(png_ptr, png_info);

    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
}


int main(){
    std::filesystem::path path = std::filesystem::current_path();
    ExtractData extract_ = ExtractData();
    // extract_.bound.x1=20;
    // extract_.bound.x2=21;
    // extract_.bound.y1=50;
    // extract_.bound.y2=51;
    extract_.bound = Rect();
    extract_.date.year=1990;
    extract_.date.month=1;
    extract_.date.day=15;
    extract_.date.hour=12;
    extract_.data_name = "2T";
    ValueByCoord* values;
    long count=1;
    unsigned long pos = 0;

    ColorAtValue violet_220 = {{102,0,214},220};
    ColorAtValue blue_240 = {{0,128,255},240};
    ColorAtValue whiteblue_260 = {{204,255,255},260};
    ColorAtValue green_270 = {{0,255,0},270};
    ColorAtValue yellow_280 = {{255,255,0},280};
    ColorAtValue red_290 = {{255,0,0},280};
    std::vector<ColorAtValue> grad = {violet_220,blue_240,whiteblue_260,green_270,yellow_280,red_290};
    for(const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(path)){
        if(entry.is_regular_file())
            if(entry.path().has_extension() && (entry.path().extension()==".grib" || entry.path().extension()==".grb")){
                std::cout<<entry.path()<<std::endl;
                 GridData grid = extract(&extract_, entry.path().c_str(),&values,&count,&pos);
                //ValuesGrid grid = extract_ptr(&extract_, entry.path().c_str(),&count,&pos);
                // for(size_t i=0;i<grid.nxny;++i){
                //     //std::cout<<"At lat:"<<values[i].lat<<"; lon:"<<values[i].lon<<" value:"<<values[i].value<<std::endl;
                //     if(values[i].lon = 0 || values[i].lat==0)
                //         std::cout<<"Invalid output"<<std::endl;
                    
                // }

                PngOut("temp_1990011512.png",values,grid.nx,grid.ny,grad);
            }
        
    }
    
}