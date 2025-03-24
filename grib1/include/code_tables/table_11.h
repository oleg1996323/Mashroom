#pragma once

#pragma pack(push,1)
#ifdef __cplusplus
    struct Flag
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
    };
#else
    typedef struct
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
    }Flag;
#endif
#pragma pack(pop)