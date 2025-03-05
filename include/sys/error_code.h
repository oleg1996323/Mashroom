#pragma once
#include <cstddef>
#include <cinttypes>
#include <string>

enum class ErrorCodeLog : uint{
    CREATE_DIR_X1_DENIED,
    BIN_FMT_FILE_MISS_IN_DIR_X1,
    FILE_X1_PERM_DENIED,
    TRY_OPEN_NOT_FILE_X1,
    CANNOT_ACCESS_PATH_X1
};

enum class ErrorCode : uint8_t{
    NONE = 0,
    INTERNAL_ERROR,
    INCORRECT_DATE_INTERVAL,
    INCORRECT_COORD
};