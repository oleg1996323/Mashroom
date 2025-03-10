#pragma once
#include <vector>
#include <string_view>

void check_parse(const std::vector<std::string_view>& input);
std::vector<std::string_view> commands_from_check_parse(const std::vector<std::string_view>& input);