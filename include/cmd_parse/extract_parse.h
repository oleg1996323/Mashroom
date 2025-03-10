#pragma once
#include <vector>
#include <string>
#include <string_view>

void extract_parse(const std::vector<std::string_view>& input);
std::vector<std::string_view> commands_from_extract_parse(const std::vector<std::string_view>& input);