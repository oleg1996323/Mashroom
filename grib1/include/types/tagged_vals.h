#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include "types/date.h"
#include "types/rect.h"
#include "types/coord.h"
#include "sections/binary/def.h"

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
    size_t capacity;
    const char* tag;
    StampVal* values;
}TaggedVal;

#define TaggedVal(...) (TaggedVal){.sz=0,.capacity=0,.tag=NULL,.values=NULL,## __VA_ARGS__}

typedef struct TAGGED_VALUES{
    size_t sz;
    size_t capacity;
    TaggedVal* vals;
}TaggedValues;

#define TaggedValues(...) (TaggedValues){.sz=0,.vals=NULL,## __VA_ARGS__}

#define EXTRACT_DATA(...) ((ExtractData) { .date = Date(), .bound = Rect(), .type = ALL_TYPE_DATA, ##__VA_ARGS__ })
#define VALUESGRID(...) ((ValuesGrid) { .nx = -1, .ny = -1, .values = NULL, ##__VA_ARGS__ })

TaggedVal* find_tagged_val_by_tag(TaggedValues* tagged_vals, const char* tag);
extern void delete_values(TaggedValues tag_vals);
extern void delete_values_by_ptr(TaggedValues* tag_vals);