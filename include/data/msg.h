#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include "OsterLib/byte_order.h"
#include "grib1/code_tables.h"
#include "grib1/sections.h"
#include "def.h"
#include "OsterLib/types/time_interval.h"
#include "grib1/def.h"
#include "grib1/error.h"
#include "OsterLib/boost_functional/json.h"
#include <stdexcept>
#include <expected>

namespace data{

template<Data_t T,Data_f F>
struct FileMsg{
    static_assert(false,"Not implemented");
};

template<>
struct FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>
{
    std::shared_ptr<GridInfo> grid_data;
    utc_tp_t<std::chrono::seconds> date;
    size_t buf_pos_;
    TimeForecast t_unit;
    Organization center;
    Level level_;
    uint32_t msg_sz_ = 0;
    uint8_t parameter = 0;
    uint8_t table_version = 0;
    std::error_code err_;

    FileMsg(GridInfo&& grid_data_,
        utc_tp_t<std::chrono::seconds>&& date_,
        ptrdiff_t msg_buf_pos,
        uint32_t msg_size,
        uint8_t parameter_,
        TimeForecast t_unit_,
        Organization center_,
        uint8_t table_version_,
        Level level,
        std::error_code err):
        grid_data(std::make_shared<GridInfo>(std::move(grid_data_))),
        date(std::move(date_)),
        buf_pos_(msg_buf_pos),
        msg_sz_(msg_size),
        parameter(parameter_),
        t_unit(t_unit_),
        center(center_),
        level_(level),
        table_version(table_version_),
        err_(err){}
    FileMsg(const GridInfo& grid_data_,
        const utc_tp_t<std::chrono::seconds>& date_,
        ptrdiff_t msg_buf_pos,
        uint32_t msg_size,
        uint8_t parameter_,
        TimeForecast t_unit_,
        Organization center_,
        uint8_t table_version_,
        Level level,
        std::error_code err):
        grid_data(std::make_shared<GridInfo>(grid_data_)),
        date(date_),
        buf_pos_(msg_buf_pos),
        msg_sz_(msg_size),
        parameter(parameter_),
        t_unit(t_unit_),
        center(center_),
        level_(level),
        table_version(table_version_),
        err_(err){}
    FileMsg() = default;
};

}

template<>
std::expected<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json<data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& val);

template<>
boost::json::value to_json(const data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val);

// using FileMsgVariant = std::variant<std::monostate,data::FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;

// template<>
// std::expected<FileMsgVariant,std::exception> from_json<FileMsgVariant>(const boost::json::value& val);

// template<>
// boost::json::value to_json(const FileMsgVariant& val);
