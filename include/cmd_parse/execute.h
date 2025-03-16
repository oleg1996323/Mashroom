#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <ranges>
#ifdef __cplusplus
    extern "C"{
        #include "capitalize.h"
        #include "decode.h"
    }
#endif

void execute(const std::vector<std::string_view>& argv);