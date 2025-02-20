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
    Date date;
    Coord coord;
    const char* data_name;
}ExtractDataCoord;

typedef struct{
    int nx;
    int ny;
    float* values;
}ValuesGrid;

typedef struct STAMP_VAL{
    Date time;
    Coord coord;
    float val;
}StampVal;

#define StampVal(...) (StampVal){.time=Date(),.coord=Coord(),.val=UNDEFINED,## __VA_ARGS__}

typedef struct TAGGED_VAL{
    size_t sz;
    const char* tag;
    StampVal* values;
}TaggedVal;

#define TaggedVal(...) (TaggedVal){.sz=0,.tag=NULL,.values=NULL,## __VA_ARGS__}

typedef struct TAGGED_VALUES{
    size_t sz;
    size_t capacity;
    TaggedVal* vals;
}TaggedValues;

#define TaggedValues(...) (TaggedValues){.sz=0,.vals=NULL,## __VA_ARGS__}

#define EXTRACT_DATA(...) ((ExtractData) { .date = Date(), .bound = Rect(), .type = ALL_TYPE_DATA, ##__VA_ARGS__ })
#define VALUESGRID(...) ((ValuesGrid) { .nx = -1, .ny = -1, .values = NULL, ##__VA_ARGS__ })

TaggedVal* find_tagged_val_by_tag(TaggedValues* tagged_vals, const char* tag);

extern GridData extract(ExtractData* data, const char* from, ValueByCoord** values,long int* count, long unsigned* pos);
extern TaggedValues extract_val_by_coord_grib(Date dfrom, Date dto,Coord coord,const char* ffrom,long int count, long unsigned pos);
extern ValuesGrid extract_ptr(ExtractData* data, const char* from, long int* count, long unsigned* pos);