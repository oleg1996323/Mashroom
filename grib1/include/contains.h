#pragma once
#include "def.h"
#include "types/coord.h"
#include "extract.h"
#include "sections/section_1.h"

#include <stdio.h>
#ifndef __cplusplus
typedef struct Result_s
{
    ExtractData data;
    ErrorCodeData code;
}Result;
#else
struct Result
{
    ExtractData data;
    ErrorCodeData code;
};
#endif
bool contains(const char* from,unsigned long* count, unsigned long* pos, const Date* date , const Coord* coord);
Result get_from_pos(const char* from,long int* count,long unsigned* pos);
const char* get_file_error_text(ErrorCodeData err);

#ifdef __cplusplus
#include <filesystem>
namespace fs = std::filesystem;
bool contains(const fs::path& from,unsigned long& count, unsigned long& pos, const Date& date ,const Coord& coord);
Result get_from_pos(const fs::path& from,long int& count,long unsigned& pos);
const char* get_file_error_text(ErrorCodeData err);
#endif