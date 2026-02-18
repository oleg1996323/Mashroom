#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include "byte_order.h"
#include "code_tables/table_0.h"
#include "code_tables/table_2.h"
#include "API/grib1/include/sections/product/levels.h"
#include "code_tables/table_4.h"
#include "code_tables/table_5.h"
#include "sections/grid/grid.h"
#include "def.h"
#include "types/time_interval.h"
#include "grib1_def.h"
#include "boost_functional/json.h"
#include <stdexcept>
#include <expected>
#include "API/grib1/include/sections/product/time_forecast.h"

template<Data_t T,Data_f F>
struct FileMsg{
    static_assert(false,"Not implemented");
};

template<>
struct FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>
{
    std::shared_ptr<GridInfo> grid_data;
    utc_tp_t<std::chrono::seconds> date;
    ptrdiff_t buf_pos_;
    TimeForecast t_unit;
    Organization center;
    Level level_;
    uint32_t msg_sz_ = 0;
    uint8_t parameter = 0;
    uint8_t table_version = 0;
    API::ErrorData::Code<API::GRIB1>::value err_ = API::ErrorData::Code<API::GRIB1>::NONE_ERR;

    FileMsg(GridInfo&& grid_data_,
        utc_tp_t<std::chrono::seconds>&& date_,
        ptrdiff_t msg_buf_pos,
        uint32_t msg_size,
        uint8_t parameter_,
        TimeForecast t_unit_,
        Organization center_,
        uint8_t table_version_,
        Level level,
        API::ErrorData::Code<API::GRIB1>::value err):
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
        API::ErrorData::Code<API::GRIB1>::value err):
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

template<>
std::expected<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>,std::exception> from_json<FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>(const boost::json::value& val);

template<>
boost::json::value to_json(const FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>& val);

// using FileMsgVariant = std::variant<std::monostate,FileMsg<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;

// template<>
// std::expected<FileMsgVariant,std::exception> from_json<FileMsgVariant>(const boost::json::value& val);

// template<>
// boost::json::value to_json(const FileMsgVariant& val);