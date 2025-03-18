#pragma once
#include <filesystem>
#include <string>
#include <variant>
#include "cmd_parse/functions.h"
#include "functions.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#ifdef __cplusplus
    extern "C"{
        #include "PDSdate.h"
        #include "capitalize.h"
        #include "extract.h"
    }
#endif
namespace fs = std::filesystem;
struct FormatData{
    Date from;
    Date to;
    std::variant<std::monostate,Coord,Rect> coord_data;
    OrderItems order;
};
namespace format{
FormatData read(const fs::path& path);
void write(OrderItems order,const Date& from, const Date& to, Coord pos);
void write(OrderItems order,const Date& from, const Date& to, const Rect& rect);
}