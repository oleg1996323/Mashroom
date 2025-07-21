#pragma once
#include <boost/json.hpp>
#include <expected>
#include <fstream>
#include <filesystem>
#include <vector>
#include <stdexcept>

namespace fs = std::filesystem;

std::expected<boost::json::value,std::error_code> parse_json(const fs::path& path) noexcept;

template<typename T>
std::expected<T,std::exception> from_json(const boost::json::value& val);

template<typename T>
boost::json::value to_json(const T& val);