#pragma once
#include <string_view>
#include <vector>
#include <string>
#include <sys/error_code.h>

ErrorCode capitalize_parse(const std::vector<std::string_view>& input);
std::vector<std::string_view> commands_from_capitalize_parse(const std::vector<std::string_view>& input,ErrorCode& err);