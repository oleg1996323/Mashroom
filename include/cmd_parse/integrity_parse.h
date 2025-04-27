#pragma once
#include <vector>
#include <string_view>
#include <sys/error_code.h>

ErrorCode integrity_parse(const std::vector<std::string_view>& input);
std::vector<std::string_view> commands_from_integrity_parse(const std::vector<std::string_view>& input,ErrorCode& err);