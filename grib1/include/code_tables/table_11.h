#pragma once
#include "functional/def.h"
#pragma pack(push,1)

//Bit 3 is set to 1 to indicate that the original data were integers; when this is the case any non-zero reference values should be rounded to an integer value prior to placing in the GRIB BDS
//Bit 4 is set to 1 to indicate that bits 5 to 12 are contained in octet 14 of the data section.
//Although GRIB is not capable of representing a matrix of data values at each grid point, the meaning of bit 6 is retained in anticipation of a future capability.
//When secondary bit maps are present in the data (used in association with second order packing) this is indicated by setting bit 7 to 1.
//When octet 14 contains the extended flag information octets 12 and 13 will also contain "special" information; the actual data will begin in a subsequent octet. See above.
STRUCT_BEG(Flag)
{
    int spherical_harm_coefs:1;
    int complex_pack:1;
    int int_values:1;
    int oct_14_flag_bits:1;
    int matrix:1;
    int second_bmp:1;
    int second_order_diff_widths:1;
}
STRUCT_END(Flag)
#pragma pack(pop)