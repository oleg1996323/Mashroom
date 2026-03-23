#pragma once
#include "grib1_def.h"
#include "api_types.h"


namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace API{
template<API::TYPES T>
struct Message;
}