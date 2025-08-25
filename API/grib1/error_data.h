#pragma once
#include <array>
#include <string_view>
#include "common/error_data.h"

namespace ErrorData{

enum Code{
    NONE_ERR,
    OPEN_ERROR_X1,
    READ_POS_X1,
    MEMORY_ERROR_X1,
    BAD_FILE_X1,
    MISS_GRIB_REC,
    RUN_OUT,
    READ_END_ERR,
    LEN_UNCONSIST,
    MISS_END_SECTION,
    DATA_EMPTY
};
}

constexpr std::array<std::string_view,11> err_code_data={
    "No error",
    "Error at openning file: {}",
    "Read position error at file: {}",
    "Memory allocation error (not enough memory)",
    "Corrupted file",
    "Missing grib record",
    "Ran out of memory",
    "Could not read to end of record",
    "Ran out of data or bad file",
    "Len of grib message is inconsistent",
    "End section missed"
};