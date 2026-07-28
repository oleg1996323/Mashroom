#pragma once
#include <cstdint>
#include <vector>
#include <ranges>
#include <functional>
#include "OsterLib/byte_read.h"
#include "def.h"
#include <span>
#include <stdexcept>
struct IndicatorSection{
    unsigned char* buf_;

    IndicatorSection(unsigned char* buffer):buf_(buffer){}

    unsigned char grib_version() const noexcept;
    unsigned long message_length() const noexcept;
};