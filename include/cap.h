#pragma once
#ifdef __cplusplus
extern "C"{
    #include <decode.h>
    #include <capitalize.h>
    #include <extract.h>
    #include <interpolation.h>
    #include <structures.h>
    #include <contains.h>
}
#endif

#include <filesystem>
#include <string>

std::string get_fmt(const OrderItems&);
void cap(const std::filesystem::path& root,const std::filesystem::path& from,const OrderItems& order);