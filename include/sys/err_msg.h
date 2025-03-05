#pragma once
#include <array>
#include "error_code.h"

constexpr std::array<const char*,5> err_msg = {
    "Directory \"{}\" creation denied",
    "Missing \"format.bin\" file in destination directory \"{}\"",
    "File \"{}\" access denied by permissions",
    "Trying to open a not-file \"{}\"",
    "Cannot access path: \"{}\""
};