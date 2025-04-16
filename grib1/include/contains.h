#pragma once
#include "def.h"
#include "types/coord.h"
#include "sections/section_1.h"
#include "code_tables/table_6.h"
#include "types/data_common.h"
#include <filesystem>
namespace fs = std::filesystem;

bool contains(const fs::path& from,const Date& date ,const Coord& coord,
    std::optional<RepresentationType> grid_type = {},
    std::optional<Organization> center = {},
    std::optional<uint8_t> table_version = {},
    std::optional<TimeFrame> fcst = {});

bool contains(const fs::path& from,const Date& date ,const Coord& coord,
    const CommonDataProperties& data,std::optional<RepresentationType> grid_type = {});

bool contains(const fs::path& from,const Date& date ,const Coord& coord,
    Organization center, uint8_t table_version, uint8_t parameter,std::optional<RepresentationType> grid_type = {});