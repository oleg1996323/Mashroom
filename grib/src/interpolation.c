#include "interpolation.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

Array_2D bilinear_interpolation(Array_2D data, int x_sz, int y_sz){
    if(!data.data || data.dx<=0 || data.dy<=0 || data.nx<=0 || data.ny<=0){
        return Array_2D();
    }
    if(x_sz == data.nx && y_sz==data.ny){
        return data;
    }
    else{
        Array_2D result = Array_2D();
        size_t total_assert;
        if(!(result.data = (float*)malloc(sizeof(float)*(x_sz-1)*(y_sz-1)))){
            fprintf(stderr,"Error with memory");
            exit(1);
        }
        else{
            total_assert= (x_sz-1)*(y_sz-1);
            result.dx = data.dx* data.nx/x_sz;
            result.dy = data.dy* data.ny/y_sz;
            result.nx = x_sz-1;
            result.ny = y_sz-1;
            float old_val = 0;
            for(size_t i = 0;i<result.ny;++i){
                float y2 = ((i*data.ny)/result.ny+1)*data.dy;
                float y1 = ((i*data.ny)/result.ny)*data.dy;
                float y = (float)i*result.dy;
                for(size_t j = 0;j<result.nx;++j){
                    float x = (float)j*result.dx;
                    float x2 = ((j*data.nx)/result.nx+1)*data.dx;
                    float x1 = ((j*data.nx)/result.nx)*data.dx;
                    size_t pos11 = data.nx*(i*data.ny/result.ny)+(j*data.nx)/result.nx;
                    size_t pos21 = data.nx*(i*data.ny/result.ny)+(j*data.nx)/result.nx+1;
                    size_t pos12 = data.nx*((i+1)*data.ny/result.ny)+(j*data.nx)/result.nx;
                    size_t pos22 = data.nx*((i+1)*data.ny/result.ny)+(j*data.nx)/result.nx+1; 
                    float Q11 = data.data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)];
                    float Q12 = data.data[data.nx*(i*data.ny/result.ny+1)+(j*data.nx/result.nx)];
                    float Q21 = data.data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx+1)];
                    float Q22 = data.data[data.nx*(i*data.ny/result.ny+1)+(j*data.nx/result.nx+1)];
                    result.data[j+result.nx*i] = (x2-x)*(y2-y)/(x2-x1)/(y2-y1)*Q11+
                    (x-x1)*(y2-y)/(x2-x1)/(y2-y1)*Q21+
                    (x2-x)*(y-y1)/(x2-x1)/(y2-y1)*Q12+
                    (x-x1)*(y-y1)/(x2-x1)/(y2-y1)*Q22;
                    assert(old_val!=result.data[j+result.nx*i]);
                    // assert(result.data[j+result.nx*i]<330);
                    // assert(result.data[j+result.nx*i]>250);
                    if(i>0)
                        assert(old_val!=result.data[j+result.nx*(i-1)]);
                    if(i>0 && j>0)
                        assert(old_val!=result.data[j-1+result.nx*(i-1)]);
                    // printf("%lu\n",j+result.nx*i);
                    // printf("Lat1: %.3f\tLon1: %.3f\tLat2: %.3f\tLon2: %.3f\nval11: %.3f\tval12: %.3f\tval21: %.3f\tval22: %.3f\nResult: %.f\n",
                    // y1,x1,y2,x2,Q11,Q12,Q21,Q22,
                    // result.data[j+result.nx*i]);
                    --total_assert;
                }
            }
            assert(total_assert==0);
        }
        return result;
    }
}
Array_2D bicubic_interpolation(Array_2D data, float x_discretion, float y_discretion){

}

float lin_interp_between(float y1, float x1, float y2, float x2, float x_value){
    if(x_value!=x1)
        return (y2-y1)/(x2 - x1)*
            (x_value - x1);
    else return y1;
}

Array_1D linear_interpolation(Array_1D data, float discretion){
    if(!data.data || data.diff<=0 || data.sz<=0 || discretion<=0){
        return Array_1D();
    }
    Array_1D result = Array_1D();
    if(discretion == data.diff){
        return data;
    }
    else{
        if(!(result.data = (float*)malloc(sizeof(float)*data.sz/discretion))){
            fprintf(stderr,"Error with memory");
            exit(1);
        }
        else{
            result.diff = discretion;
            result.sz = (int)(data.diff*(float)data.sz/discretion);
        }
    }

    for(size_t i = 0;i<result.sz-1;++i){
        float y2 = data.data[i*data.sz/result.sz+1];
        float y1 = data.data[i*data.sz/result.sz];
        float x2 = (i*data.sz/result.sz+1)*data.diff;
        float x1 = (i*data.sz/result.sz)*data.diff;
        result.data[i] = lin_interp_between(data.data[(i*data.sz)/result.sz],
                        (float)((i*data.sz)/result.sz)*data.diff,
                        data.data[(i*data.sz)/result.sz+1],
                        (float)((i*data.sz)/result.sz+1)*data.diff,
                        (float)i*result.diff);
    }
}