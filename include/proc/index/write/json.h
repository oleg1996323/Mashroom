#pragma once
#include <expected>
#include <vector>
#include <stdexcept>
#include "data/msg.h"
#include "boost_functional/json.h"

template<>
std::expected<std::vector<GribMsgDataInfo>,std::exception> from_json(const boost::json::value& val);

template<>
boost::json::value to_json(const std::vector<GribMsgDataInfo>& val);