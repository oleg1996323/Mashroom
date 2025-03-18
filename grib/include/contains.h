#pragma once
#include <stdio.h>
#include "def.h"
#include "PDSdate.h"
#include "func.h"
#include "coords.h"
#include "extract.h"

typedef struct Result_s
{
    ExtractData data;
    ErrorCodeData code;
}Result;

bool contains(const char* from,long int* count, long int* pos, Date* date ,Coord* coord);

extern Result get_from_pos(const char* from,long int* count,long unsigned* pos);
extern const char* get_file_error_text(ErrorCodeData err);