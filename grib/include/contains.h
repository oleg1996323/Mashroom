#pragma once
#include <stdio.h>
#include "def.h"
#include "PDSdate.h"
#include "func.h"
#include "coords.h"
#include "extract.h"

typedef enum ERROR_CODE_DATA{
    NONE_ERR,
    OPEN_ERROR,
    READ_POS,
    MEMORY,
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

typedef struct Result_s
{
    ExtractData data;
    ErrorCodeData code;
}Result;

bool contains(const char* from,long int* count, long int* pos, Date* date ,Coord* coord);

extern Result get_from_pos(const char* from,long int* count,long unsigned* pos);
extern const char* get_file_error_text(ErrorCodeData err);

extern const char* get_file_error_text(ErrorCodeData err);