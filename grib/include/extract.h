#pragma once
#include <stdio.h>
#include "def.h"
#include "PDSdate.h"
#include "func.h"


struct EXTRACT_DATA
{
    struct DATE date;
    struct RECT bound;
}Extract_Data;

#define EXTRACT_DATA(...) ((Extract_Data) { (.date = {-1,-1,-1,-1}), (.bound = {-1,-1,-1,-1}), ##__VA_ARGS__ })

extern int extract(struct EXTRACT_DATA* data, const char* from);