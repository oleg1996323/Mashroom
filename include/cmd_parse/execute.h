#pragma once
#include <vector>
#include <string>
#include <string_view>
#ifdef __cplusplus
    extern "C"{
        #include "capitalize.h"
        #include "decode.h"
    }
#endif

void execute(std::vector<std::string_view> argv);