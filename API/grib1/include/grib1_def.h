#pragma once
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "sections/section_2.h"
#include "common/error_data.h"

#ifndef PATH_MAX
#define PATH_MAX UCHAR_MAX*8
#endif

static unsigned int mask[] = {0,1,3,7,15,31,63,127,255};
static unsigned int map_masks[8] = {128, 64, 32, 16, 8, 4, 2, 1};
static double shift[9] = {1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0};
