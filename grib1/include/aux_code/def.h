#pragma once
#include <cstdint>

#ifdef __cplusplus
#define STRUCT_BEG(x) struct x
#define STRUCT_END(x) ;
#define TEMPLATE_STRUCT_BEG(str_name,T) template<typename T> \
                                        struct str_name
#define TEMPLATE_STRUCT_END(x) ;
#define SPECIFIED_TEMPLATE_STRUCT_BEG(str_name,T) template<> \
                                                struct str_name<T>
#define SPECIFIED_TEMPLATE_STRUCT_END(x) ;                                             
#else 
#define STRUCT_BEG(x) typedef struct
#define STRUCT_END(x) x;
#endif