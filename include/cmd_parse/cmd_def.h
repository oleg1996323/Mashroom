#pragma once

#ifndef MODE_ENUM
#define MODE_ENUM
enum class MODE{
    NONE,
    CHECK_ALL_IN_PERIOD,
    CAPITALIZE,
    EXTRACT,
    CONFIG,
    HELP
};
#endif

#ifndef DATA_EXTRACT_MODE
#define DATA_EXTRACT_MODE
enum class DataExtractMode{
    UNDEF,
    POSITION,
    RECT
};
#endif