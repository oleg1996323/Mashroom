#pragma once
#include <float.h>

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