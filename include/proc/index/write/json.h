#pragma once
#include <expected>
#include <vector>
#include <stdexcept>
#include "data/msg.h"
#include "boost_functional/json.h"

template<>
std::expected<std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>,std::exception> from_json(const boost::json::value& val);

template<>
boost::json::value to_json(const std::vector<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>& val);