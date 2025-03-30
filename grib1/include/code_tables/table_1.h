#pragma once

#ifdef __cplusplus
struct Section2_3_flag{
    unsigned sec2_inc;
    unsigned sec3_inc;
    unsigned :6;
};
#else
typedef struct{
    unsigned sec2_inc:1;
    unsigned sec3_inc:1;
    unsigned :6;
}Section2_3_flag;
#endif