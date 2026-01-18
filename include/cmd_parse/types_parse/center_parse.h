#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "sys/error_code.h"
#include <thread>
#include "cmd_def.h"
#include "cmd_parse/functions.h"
#include "sys/error_print.h"
#include "functions.h"
#include "code_tables/table_0.h"
#include "proc/interfaces/abstractsearchprocess.h"
#include "types/time_interval.h"
#include "grid_type_parse.h"

#include "cmd_parse/cmd_def.h"
#include "sections/section_1.h"
#include "param_tablev_parse.h"
#include "path_parse.h"
#include "coord_parse.h"
#include "time_separation_parse.h"
#include <expected>

namespace parse{
    std::expected<Organization,ErrorCode> center_notifier(const std::vector<std::string>& input) noexcept;
}