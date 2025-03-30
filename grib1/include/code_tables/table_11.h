#pragma once
#include "aux_code/def.h"
#pragma pack(push,1)
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