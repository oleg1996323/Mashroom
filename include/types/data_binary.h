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
#include "types/data_info.h"
namespace fs = std::filesystem;

enum class FormatTokens{
    DateFrom,
    DateTo,
    Coord,
    Capitalize_data,
    Capitalize_format
};
enum class BinaryGribInfoType{
    REF = 0,
    FULL_DATA
};

using namespace std::string_literals;
class BinaryGribInfo{
    GribDataInfo data_ = GribDataInfo();
    std::string order_;
    DataFormat fmt_;
    BinaryGribInfo() = default;
    public:
    BinaryGribInfo(GribDataInfo&& data, const std::string& order):
    data_(std::move(data)),order_(order){}
    BinaryGribInfo(const fs::path& path);
    static BinaryGribInfo read(const fs::path& path);
    // void write(OrderItems order,const Date& from, const Date& to, Coord pos);
    void write(const fs::path& dir, BinaryGribInfoType bin_info_type);
    const GribDataInfo& data() const{
        return data_;
    }
    const std::string& order() const{
        return order_;
    }
    DataFormat format() const{
        return fmt_;
    }
};