#ifdef __cplusplus
extern "C"{
    #include <decode.h>
    #include <capitalize.h>
    #include <extract.h>
    #include <interpolation.h>
    #include <structures.h>
}
#endif
#include <dlfcn.h>
#include <memory>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <numeric>
#include "color.h"
#include "png_out.h"

#include <cassert>

int main(){
    std::filesystem::path path = std::filesystem::current_path();
    ExtractData extract_ = ExtractData();
    // extract_.bound.x1=20;
    // extract_.bound.x2=21;
    // extract_.bound.y1=50;
    // extract_.bound.y2=51;
    extract_.bound = Rect();
    extract_.date.year=2017;
    extract_.date.month=5;
    extract_.date.day=15;
    extract_.date.hour=12;
    extract_.data_name = "2T";
    ValueByCoord* values;
    long count=1;
    unsigned long pos = 0;

    ColorAtValue<RGB<16>> violet_220 = {{102,0,214},200};
    ColorAtValue<RGB<16>> blue_240 = {{0,128,UCHAR_MAX},220};
    ColorAtValue<RGB<16>> whiteblue_250 = {{204,UCHAR_MAX,UCHAR_MAX},240};
    ColorAtValue<RGB<16>> lightgreen_260 = {{102,UCHAR_MAX,102},260};
    ColorAtValue<RGB<16>> green_270 = {{0,UCHAR_MAX,0},280};
    ColorAtValue<RGB<16>> yellow_280 = {{UCHAR_MAX,UCHAR_MAX,0},300};
    ColorAtValue<RGB<16>> red_290 = {{UCHAR_MAX,0,0},320};
    //ColorAtValue blue_240 = {{0,128,UCHAR_MAX},UCHAR_MAX};
    //ColorAtValue red_290 = {{UCHAR_MAX,0,0},320};
    violet_220.color=violet_220.color*((uint8_t)255);
    std::vector<ColorAtValue<RGB<16>>> grad = {violet_220,blue_240,whiteblue_250,lightgreen_260,green_270,yellow_280,red_290};
    //std::vector<ColorAtValue> grad = {blue_240,red_290};
    for(const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(path)){
        if(entry.is_regular_file())
            if(entry.path().has_extension() && (entry.path().extension()==".grib" || entry.path().extension()==".grb")){

                //std::cout<<entry.path()<<std::endl;
                GridData grid = extract(&extract_, entry.path().c_str(),&values,&count,&pos);
                Array_2D arr;
                count = 1;
                pos = 0;
                auto res = extract_ptr(&extract_,entry.path().c_str(),&count,&pos);
                arr.data = res.values;
                arr.nx = res.nx;
                arr.ny = res.ny;
                arr.dx = grid.dx;
                arr.dy = grid.dy;
                Array_2D result = bilinear_interpolation(arr,res.nx*4,res.ny*4);
                PngOutGray("temp_1990011512_X4xX4_gray.png",result.data,Size(result.nx,result.ny));
                PngOutRGBGradient<8>("temp_1990011512_X4xX4.png",result.data,Size(result.nx,result.ny),grad,true);
                //PngOut("temp_1990011512_X4xX4.png",result.data,result.nx,result.ny,grad);
            }
        
    }
    
}