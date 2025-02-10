#pragma once
#include <stdio.h>
#include "def.h"
#include "PDSdate.h"
#include "func.h"
#include "coords.h"
#include "extract.h"

enum ErrorCodes{
    NONE_ERR,
    OPEN,ERROR,
    READ_POS,
    MEMORY,
    BAD_FILE,
    MISS_GRIB_REC,
    RUN_OUT,
    READ_END_ERR,
    LEN_UNCONSIST,
    MISS_END_SECTION
};

typedef struct Result_s
{
    ExtractData data;
    enum ErrorCodes code;
}Result;

bool contains(const char* from,long int* count, long int* pos, Date* date ,Coord* coord);

Result get_from_pos(const char* from,long int* count,long unsigned* pos);