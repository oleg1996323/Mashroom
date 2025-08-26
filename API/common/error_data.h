#pragma once
#include <array>
#include <string_view>
#include "api_types.h"

namespace API::ErrorData{

    template<API::TYPES TYPE>
    struct Code{
        enum value{};
        static_assert(false,"Undefined structure");
    };

    template<API::TYPES TYPE>
    using ErrorCode = Code<TYPE>::value;

    template<>
    struct Code<API::TYPES::COMMON>{
        enum value{
            NONE_ERR,
            OPEN_ERROR_X1,
            MEMORY_ERROR,
            BAD_FILE_X1,
            RUN_OUT_X1,
            DATA_EMPTY_X1
        };

        static constexpr std::array<std::string_view,6> err_code_data={
            "No error",
            "Error at openning file: {}",
            "Memory allocation error (not enough memory)",            
            "Corrupted file: {}",
            "Ran out of memory",
            "Data not found at file: {}"
        };
    };
}