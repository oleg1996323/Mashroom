#pragma once
#include <stdio.h>
#include "def.h"
#include "PDSdate.h"
#include "func.h"
#include "coords.h"
#include "extract.h"

bool contains(const char* from,long int* count, long int* pos, Date* date ,Coord* coord);

ExtractData get_from_pos(const char* from,long int* count,long unsigned* pos);