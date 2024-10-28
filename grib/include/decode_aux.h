#pragma once
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

struct VALID_CAP_FMT{
    bool COORD;
    bool YEAR;
    bool MONTH;
    bool DAY;
    bool HOUR;
};

bool change_directory(const char* dir);

bool check(char ch, struct VALID_CAP_FMT* valid);