#pragma once
#include <float.h>
#include <memory.h>
#include "structures.h"
#include <stdbool.h>

extern float lin_interp_between(float y1, float x1, float y2, float x2, float x_value);
extern Array_2D bilinear_interpolation(Array_2D, int, int);
extern Array_2D bicubic_interpolation(Array_2D, float, float);
extern Array_1D linear_interpolation(Array_1D, float);