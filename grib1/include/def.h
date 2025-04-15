#pragma once
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "sections/section_2.h"
#include "types/date.h"

#ifndef PATH_MAX
#define PATH_MAX UCHAR_MAX*8
#endif


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

typedef struct VALUE_BY_COORD{
    float value;
    double lat;
    double lon;
}ValueByCoord;

typedef struct VALUES{
    Date date;
    ValueByCoord* values_by_coord;
}Values;