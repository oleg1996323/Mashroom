#pragma once

enum class TIME_UNIT_ENUM {
    HOUR,
    DAY,
    MONTH,
    YEAR,
    LATITUDE,
    LONGITUDE,
    LATLON
};

const std::unordered_map<TIME_UNIT_ENUM,std::unordered_set<std::string>>& token_to_txt() noexcept;
const std::unordered_map<std::string,TIME_UNIT_ENUM>& txt_to_token() noexcept;
std::vector<TIME_UNIT_ENUM> input_to_token_sequence(const std::string&);
bool check_format(std::string_view fmt);
bool check_format(const std::vector<TIME_UNIT_ENUM>& tokens);