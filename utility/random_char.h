#pragma once
#include <random>

auto getRandomChar = []()->char
{
    static char c = 'A' + rand()%24;
    return c;    
};