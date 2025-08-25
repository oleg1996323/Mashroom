#pragma once
#include <cstdint>
#include "byte_read.h"
#include "def.h"

constexpr unsigned sec_0_min_sz = 8;
constexpr unsigned sec_1_min_sz = 40;
constexpr unsigned sec_2_min_sz = 52;
constexpr unsigned grib1_header_min_sz = sec_0_min_sz+sec_1_min_sz+sec_2_min_sz;