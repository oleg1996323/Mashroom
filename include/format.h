#pragma once
#include <filesystem>
#include <string>
#include <variant>
#include "cmd_parse/functions.h"
#include "functions.h"
#include "sys/error_code.h"
#include "sys/error_print.h"
#include "sys/format_def.h"

#include "sections/section_1.h"
#include "capitalize.h"
#include "extract.h"
namespace fs = std::filesystem;

enum class FormatTokens{
    DateFrom,
    DateTo,
    Coord,
    Rect,
    Capitalize_data,
    Capitalize_format
};
// struct FormatBinData{
//     Date from;
//     Date to;
//     std::variant<std::monostate,Coord,Rect> coord_data;
//     OrderItems order;
// };
struct FormatBinData{
    GribDataInfo data_ = GribDataInfo();
    OrderItems order_ = OrderItems();
};
namespace format{
    using namespace std::string_literals;
    FormatBinData read(const fs::path& path);
    // void write(OrderItems order,const Date& from, const Date& to, Coord pos);
    void write(const fs::path& dir, const OrderItems& order,const GribDataInfo& data);
    void write(const fs::path& dir,const FormatBinData& data);
}