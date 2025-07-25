#include "interpolation.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

Array_2D bilinear_interpolation(const Array_2D& data, int x_sz, int y_sz){
    FILE* log = fopen("log.txt","w");
    if(!log)
        exit(1);
    if(data.dx<=0 || data.dy<=0 || data.nx<=0 || data.ny<=0){
        return Array_2D();
    }
    if(x_sz == data.nx && y_sz==data.ny){
        return data;
    }
    else{
        Array_2D result = Array_2D();
        result.resize(x_sz*y_sz);
        size_t total_assert = x_sz*y_sz;
        {
            result.dx = data.dx* data.nx/x_sz;
            result.dy = data.dy* data.ny/y_sz;
            result.nx = x_sz;
            result.ny = y_sz;
            float old_val = 0;
            for(size_t i = 0;i<result.ny;++i){
                float y2 = ((i*data.ny)/result.ny+1)*data.dy;
                float y1 = ((i*data.ny)/result.ny)*data.dy;
                float y = (float)i*result.dy;
                for(size_t j = 0;j<result.nx;++j){
                    float x = (float)j*result.dx;
                    float x2 = ((j*data.nx)/result.nx+1)*data.dx;
                    float x1 = ((j*data.nx)/result.nx)*data.dx;
                    if(j>result.nx-result.nx/data.nx && i>result.ny-result.ny/data.ny){
                        result[j+result.nx*i] = data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)];
                        --total_assert;
                        if(result[j+result.nx*i]<200)
                            fprintf(log,"Pos: %lu, i: %lu, j: %lu value: %.3f\n",j+result.nx*i, i, j,result[j+result.nx*i]);
                        continue;
                    }
                    if(j>result.nx-result.nx/data.nx){
                        result[j+result.nx*i] = lin_interp_between(data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)],
                                                                        y1,
                                                                        data[data.nx*(i*data.ny/result.ny+1)+(j*data.nx/result.nx)],
                                                                        y2,
                                                                        y);
                        --total_assert;
                        if(result[j+result.nx*i]<200)
                            fprintf(log,"Pos: %lu, i: %lu, j: %lu value: %.3f\n",j+result.nx*i, i, j,result[j+result.nx*i]);
                        continue;
                    }
                    if(i>result.ny-result.ny/data.ny){
                        result[j+result.nx*i] = lin_interp_between(data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)],
                                                                        x1,
                                                                        data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx+1)],
                                                                        x2,
                                                                        x);
                        --total_assert;
                        if(result[j+result.nx*i]<200)
                            fprintf(log,"Pos: %lu, i: %lu, j: %lu value: %.3f\n",j+result.nx*i, i, j,result[j+result.nx*i]);
                        continue;
                    }
                    size_t pos11 = data.nx*(i*data.ny/result.ny)+(j*data.nx)/result.nx;
                    size_t pos21 = data.nx*(i*data.ny/result.ny)+(j*data.nx)/result.nx+1;
                    size_t pos12 = data.nx*((i+1)*data.ny/result.ny)+(j*data.nx)/result.nx;
                    size_t pos22 = data.nx*((i+1)*data.ny/result.ny)+(j*data.nx)/result.nx+1; 
                    float Q11 = data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)];
                    float Q12 = data[data.nx*((i+1)*data.ny/result.ny)+(j*data.nx/result.nx)];
                    float Q21 = data[data.nx*(i*data.ny/result.ny)+((j+1)*data.nx/result.nx)];
                    float Q22 = data[data.nx*((i+1)*data.ny/result.ny)+((j+1)*data.nx/result.nx)];
                    result[j+result.nx*i] = (x2-x)*(y2-y)/(x2-x1)/(y2-y1)*Q11+
                    (x-x1)*(y2-y)/(x2-x1)/(y2-y1)*Q21+
                    (x2-x)*(y-y1)/(x2-x1)/(y2-y1)*Q12+
                    (x-x1)*(y-y1)/(x2-x1)/(y2-y1)*Q22;
                    assert(old_val!=result[j+result.nx*i]);
                    // assert(result[j+result.nx*i]<330);
                    // assert(result[j+result.nx*i]>250);
                    if(i>0)
                        assert(old_val!=result[j+result.nx*(i-1)]);
                    if(i>0 && j>0)
                        assert(old_val!=result[j-1+result.nx*(i-1)]);

                    if(result[j+result.nx*i]<200)
                        fprintf(log,"Pos: %lu, i: %lu, j: %lu value: %.3f\n",j+result.nx*i, i, j,result[j+result.nx*i]);
                    // printf("Lat1: %.3f\tLon1: %.3f\tLat2: %.3f\tLon2: %.3f\nval11: %.3f\tval12: %.3f\tval21: %.3f\tval22: %.3f\nResult: %.f\n",
                    // y1,x1,y2,x2,Q11,Q12,Q21,Q22,
                    // result[j+result.nx*i]);
                    --total_assert;
                }
            }
            assert(total_assert==0);
        }
        fclose(log);
        return result;
    }
}

//need to perform
// Array_2D bicubic_interpolation(Array_2D data, int x_sz, int y_sz){
//     FILE* log = fopen("log.txt","w");
//     if(!log)
//         exit(1);
//     if(!data || data.dx<=0 || data.dy<=0 || data.nx<=0 || data.ny<=0){
//         return Array_2D();
//     }
//     if(x_sz == data.nx && y_sz==data.ny){
//         return data;
//     }
//     else{
//         Array_2D result = Array_2D();
//         size_t total_assert;
//         if(!(result = (float*)malloc(sizeof(float)*(x_sz*y_sz)))){
//             fprintf(stderr,"Error with memory");
//             exit(1);
//         }
//         else{
//             total_assert= (x_sz)*(y_sz);
//             result.dx = data.dx* data.nx/x_sz;
//             result.dy = data.dy* data.ny/y_sz;
//             result.nx = x_sz;
//             result.ny = y_sz;
//             float old_val = 0;
//             for(size_t i = 0;i<result.ny;++i){
//                 float y2 = ((i*data.ny)/result.ny+1)*data.dy;
//                 float y1 = ((i*data.ny)/result.ny)*data.dy;
//                 float y = (float)i*result.dy;
//                 for(size_t j = 0;j<result.nx;++j){
//                     float x = (float)j*result.dx;
//                     float x2 = ((j*data.nx)/result.nx+1)*data.dx;
//                     float x1 = ((j*data.nx)/result.nx)*data.dx;
//                     float x_1 = ((j*data.nx)/result.nx-1)*data.dx;
//                     float x_2 = ((j*data.nx)/result.nx-2)*data.dx;
//                     if(j>result.nx-result.nx/data.nx && i>result.ny-result.ny/data.ny){
//                         result[j+result.nx*i] = data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)];
//                         --total_assert;
//                         if(result[j+result.nx*i]<200)
//                             fprintf(log,"Pos: %lu, i: %lu, j: %lu value: %.3f\n",j+result.nx*i, i, j,result[j+result.nx*i]);
//                         continue;
//                     }
//                     if(j>result.nx-result.nx/data.nx){
//                         result[j+result.nx*i] = lin_interp_between(data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)],
//                                                                         y1,
//                                                                         data[data.nx*(i*data.ny/result.ny+1)+(j*data.nx/result.nx)],
//                                                                         y2,
//                                                                         y);
//                         --total_assert;
//                         if(result[j+result.nx*i]<200)
//                             fprintf(log,"Pos: %lu, i: %lu, j: %lu value: %.3f\n",j+result.nx*i, i, j,result[j+result.nx*i]);
//                         continue;
//                     }
//                     if(i>result.ny-result.ny/data.ny){
//                         result[j+result.nx*i] = lin_interp_between(data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)],
//                                                                         x1,
//                                                                         data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx+1)],
//                                                                         x2,
//                                                                         x);
//                         --total_assert;
//                         if(result[j+result.nx*i]<200)
//                             fprintf(log,"Pos: %lu, i: %lu, j: %lu value: %.3f\n",j+result.nx*i, i, j,result[j+result.nx*i]);
//                         continue;
//                     }
//                     size_t pos11 = data.nx*(i*data.ny/result.ny)+(j*data.nx)/result.nx;
//                     size_t pos21 = data.nx*(i*data.ny/result.ny)+(j*data.nx)/result.nx+1;
//                     size_t pos12 = data.nx*((i+1)*data.ny/result.ny)+(j*data.nx)/result.nx;
//                     size_t pos22 = data.nx*((i+1)*data.ny/result.ny)+(j*data.nx)/result.nx+1; 
//                     float Q11 = data[data.nx*(i*data.ny/result.ny)+(j*data.nx/result.nx)];
//                     float Q12 = data[data.nx*((i+1)*data.ny/result.ny)+(j*data.nx/result.nx)];
//                     float Q21 = data[data.nx*(i*data.ny/result.ny)+((j+1)*data.nx/result.nx)];
//                     float Q22 = data[data.nx*((i+1)*data.ny/result.ny)+((j+1)*data.nx/result.nx)];
//                     result[j+result.nx*i] = (x2-x)*(y2-y)/(x2-x1)/(y2-y1)*Q11+
//                     (x-x1)*(y2-y)/(x2-x1)/(y2-y1)*Q21+
//                     (x2-x)*(y-y1)/(x2-x1)/(y2-y1)*Q12+
//                     (x-x1)*(y-y1)/(x2-x1)/(y2-y1)*Q22;
//                     assert(old_val!=result[j+result.nx*i]);
//                     // assert(result[j+result.nx*i]<330);
//                     // assert(result[j+result.nx*i]>250);
//                     if(i>0)
//                         assert(old_val!=result[j+result.nx*(i-1)]);
//                     if(i>0 && j>0)
//                         assert(old_val!=result[j-1+result.nx*(i-1)]);

//                     if(result[j+result.nx*i]<200)
//                         fprintf(log,"Pos: %lu, i: %lu, j: %lu value: %.3f\n",j+result.nx*i, i, j,result[j+result.nx*i]);
//                     // printf("Lat1: %.3f\tLon1: %.3f\tLat2: %.3f\tLon2: %.3f\nval11: %.3f\tval12: %.3f\tval21: %.3f\tval22: %.3f\nResult: %.f\n",
//                     // y1,x1,y2,x2,Q11,Q12,Q21,Q22,
//                     // result[j+result.nx*i]);
//                     --total_assert;
//                 }
//             }
//             assert(total_assert==0);
//         }
//         fclose(log);
//         return result;
//     }
// }

float lin_interp_between(float y1, float x1, float y2, float x2, float x_value){
    if(x_value!=x1)
        return y1+(y2-y1)/(x2 - x1)*
            (x_value - x1);
    else return y1;
}

Array_1D linear_interpolation(const Array_1D& data, float discretion){
    if(data.diff<=0 || data.size()<=0 || discretion<=0){
        return Array_1D();
    }
    Array_1D result = Array_1D();
    if(discretion == data.diff){
        return data;
    }
    result.resize(data.size()/discretion);
    result.diff = discretion;

    for(size_t i = 0;i<result.size()-1;++i){
        float y2 = data[i*data.size()/result.size()+1];
        float y1 = data[i*data.size()/result.size()];
        float x2 = (i*data.size()/result.size()+1)*data.diff;
        float x1 = (i*data.size()/result.size())*data.diff;
        result[i] = lin_interp_between(data[(i*data.size())/result.size()],
                        (float)((i*data.size())/result.size())*data.diff,
                        data[(i*data.size())/result.size()+1],
                        (float)((i*data.size())/result.size()+1)*data.diff,
                        (float)i*result.diff);
    }
    return result;
}