#pragma once
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "sections/section_2.h"

#ifndef PATH_MAX
#define PATH_MAX UCHAR_MAX*8
#endif

static unsigned int mask[] = {0,1,3,7,15,31,63,127,255};
static unsigned int map_masks[8] = {128, 64, 32, 16, 8, 4, 2, 1};
static double shift[9] = {1.0, 2.0, 4.0, 8.0, 16.0, 32.0, 64.0, 128.0, 256.0};
typedef enum {
    NONE_ERR,
    DATA_EMPTY,
    OPEN_ERROR,
    READ_POS,
    MEMORY_ERROR,
    BAD_FILE,
    MISS_GRIB_REC,
    RUN_OUT,
    READ_END_ERR,
    LEN_UNCONSIST,
    MISS_END_SECTION
}ErrorCodeData;

static const char* err_code_data[11]={
    "Correct",
    "Error openning",
    "Read position error",
    "Memory allocation error (not enough memory)",
    "Corrupted file",
    "Missing grib record",
    "Ran out of memory",
    "Could not read to end of record",
    "Ran out of data or bad file",
    "Len of grib message is inconsistent",
    "End section missed"
};

enum class DataFormat{
    NONE,
    BINARY,
    TEXT, 
    IEEE, 
    GRIB,
    REF //add ref on message position with data in concrete file
};