#pragma once
#include <stdio.h>
#include "def.h"
#include "PDSdate.h"
#include "func.h"

typedef enum{
    ALL_TYPE_DATA,
    U10,
    V10,
    T2,
    NSSS,
    INSS,
    IEWS,
    EWSS,
    U100,
    V100,
    MLSPR
}TypeData;

typedef struct{
    Date date;
    Rect bound;
    const char* data_name;
}ExtractData;

typedef struct{
    int nx;
    int ny;
    float* values;
}ValuesGrid;

#define EXTRACT_DATA(...) ((ExtractData) { .date = Date(), .bound = Rect(), .type = ALL_TYPE_DATA, ##__VA_ARGS__ })
#define VALUESGRID(...) ((ValuesGrid) { .nx = -1, .ny = -1, .values = NULL, ##__VA_ARGS__ })

extern GridData extract(ExtractData* data, const char* from, ValueByCoord** values,long int* count, long unsigned* pos);

extern ValuesGrid extract_ptr(ExtractData* data, const char* from, long int* count, long unsigned* pos);