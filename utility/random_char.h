#pragma once
#include <random>

auto getRandomChar = []()->char
{
    std::mt19937 gen;
    gen.seed(std::rand());
    return 'A' + gen()%26;    
};