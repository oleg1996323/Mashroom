#pragma once
#include <float.h>

#ifndef __cplusplus
typedef struct{
    float* data;
    int nx;
    int ny;
    float dx;
    float dy;
}Array_2D;

#define Array_2D(...) ((Array_2D){.data=NULL,.dx=0,.dy=0,.nx=0,.ny=0,## __VA_ARGS__})

typedef struct{
    float* data;
    float diff;
    int sz;
}Array_1D;

#define Array_1D(...) ((Array_1D){.data=NULL,.diff=0,.sz=0,## __VA_ARGS__})
#else
#include <vector>

struct Array_2D:public std::vector<float>{
    int nx = 0;
    int ny = 0;
    float dx = 0;
    float dy = 0;
};
struct Array_1D:std::vector<float>{
    float diff = 0;
};
#endif