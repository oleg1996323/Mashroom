#pragma once
#include <array>
#include <string_view>
#include "common/error_data.h"

#ifdef GRIB1API
namespace API::ErrorData{
    template<>
    struct Code<API::TYPES::GRIB1>{
        enum value{
            NONE_ERR,
            OPEN_ERROR_X1,
            MEMORY_ERROR,
            BAD_FILE_X1,
            RUN_OUT_X1,
            DATA_EMPTY_X1,
            READ_POS_X1,
            MISS_GRIB_REC_X1,
            LEN_UNCONSIST_X1,
            MISS_END_SECTION_X1
        };

        static constexpr std::array<std::string_view,10> err_code_data={
            "No error",
            "Error at openning file: {}",
            "Memory allocation error (not enough memory)",            
            "Corrupted file: {}",
            "Ran out of memory",
            "Data not found at file: {}",
            "Read position error at file: {}",
            "Missing grib record",
            "Could not read the end of record",
            "Len of grib message is inconsistent"
        };
    };
}
#endif