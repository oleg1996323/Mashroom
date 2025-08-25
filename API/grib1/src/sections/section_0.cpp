#include "sections/section_0.h"

unsigned char IndicatorSection::grib_version() const noexcept{
    return buf_[7];
}
unsigned long IndicatorSection::message_length() const noexcept{
    return read_bytes<3,false>(&buf_[4]);
}