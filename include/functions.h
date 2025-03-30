#pragma once
#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "sections/section_1.h"
#include "capitalize.h"
#include "extract.h"
#include "types/date.h"

namespace fs = std::filesystem;

namespace auxiliairy{
    bool is_correct_date_interval(const Date& from, const Date& to);
    bool is_in_accessible_positions(const Coord& pos);
    Rect get_accessible_rect();
}