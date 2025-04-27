#pragma once
#include <vector>
#include <string_view>
#include <sys/error_code.h>

ErrorCode config_parse(const std::vector<std::string_view>& args);