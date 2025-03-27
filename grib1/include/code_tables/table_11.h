#pragma once
#include "aux_code/def.h"
#pragma pack(push,1)
STRUCT_BEG(Flag)
{
    unsigned spherical_harm_coefs:1;
    unsigned complex_pack:1;
    unsigned int_values:1;
    unsigned oct_14_flag_bits:1;
    unsigned :1;
    unsigned matrix:1;
    unsigned second_bmp:1;
    unsigned second_order_diff_widths:1;
    unsigned :4;
}
STRUCT_END(Flag)
#pragma pack(pop)