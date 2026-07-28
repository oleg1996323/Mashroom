#pragma once
#include <variant>
#include "web/common/msgdef.h"
#include "grib1/code_tables.h"
#include "grib1/sections.h"
#include "OsterLib/types/rect.h"
#include "data/common_data_properties.h"
#include <unordered_set>
#include <optional>
#include "definitions/def.h"

namespace network{
template<Data_t T,Data_f F>
struct IndexParameters;

template<>
struct IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>{
    std::unordered_set<CommonDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1>> common_;
    std::optional<utc_tp_t<std::chrono::seconds>> from_;
    std::optional<utc_tp_t<std::chrono::seconds>> to_;
    std::optional<DateTimeDiff> tdiff_;
    std::optional<std::pair<TimeForecast,TimeForecast::COMPARISION_TYPE>> forecast_preference_;
    std::optional<std::pair<Level,Level::COMPARISION_TYPE>> level_;
    std::optional<Lat> top_;
    std::optional<Lat> bottom_;
    std::optional<Lon> left_;
    std::optional<Lon> right_;
    std::optional<RepresentationType> grid_type_;
    IndexParameters() = default;
    IndexParameters(IndexParameters&& other) noexcept;
    IndexParameters(const IndexParameters& other) noexcept;
    IndexParameters& operator=(IndexParameters&& other) noexcept;
    IndexParameters& operator=(const IndexParameters& other) noexcept;

    IndexParameters& grid_type(RepresentationType rep) noexcept;
    IndexParameters& from(utc_tp_t<std::chrono::seconds> from) noexcept;
    IndexParameters& to(utc_tp_t<std::chrono::seconds> to) noexcept;
    IndexParameters& time_diff(DateTimeDiff diff) noexcept;
    IndexParameters& position_rect(
            std::optional<Lat> top,
            std::optional<Lat> bottom,
            std::optional<Lon> left,
            std::optional<Lon> right) noexcept;
    IndexParameters& forecast_preference(
            TimeForecast fcst,
            TimeForecast::COMPARISION_TYPE comp_type) noexcept;
    IndexParameters& level_preference(
            Level lvl,
            Level::COMPARISION_TYPE comp_type) noexcept;
    IndexParameters& top(const Lat& pos) noexcept;
    IndexParameters& bottom(const Lat& pos) noexcept;
    IndexParameters& left(const Lon& pos) noexcept;
    IndexParameters& right(const Lon& pos) noexcept;
    IndexParameters& common_data_properties(
        const std::unordered_set<
            CommonDataProperties<Data_t::TIME_SERIES,
                                Data_f::GRIB_v1>>& cmn)
                                noexcept;
    IndexParameters& add_common_data_property(
        CommonDataProperties<Data_t::TIME_SERIES,
                                Data_f::GRIB_v1> cmn)
                                noexcept;
    ///////
    const std::optional<RepresentationType>& grid_type() const noexcept;
    const std::optional<utc_tp_t<std::chrono::seconds>>& 
                                            from() const noexcept;
    const std::optional<utc_tp_t<std::chrono::seconds>>& 
                                            to() const noexcept;
    const std::optional<DateTimeDiff>& 
                                        time_diff() const noexcept;
    const std::optional<Lon>& left() const noexcept;
    const std::optional<Lon>& right() const noexcept;
    const std::optional<Lat>& top() const noexcept;
    const std::optional<Lat>& bottom() const noexcept;
    const std::optional<std::pair<TimeForecast,
            TimeForecast::COMPARISION_TYPE>>& 
                    forecast_preference() const noexcept;
    const std::optional<std::pair<Level,Level::COMPARISION_TYPE>>& 
                        level_preference() const noexcept;
    const std::unordered_set<
        CommonDataProperties<
            Data_t::TIME_SERIES,
            Data_f::GRIB_v1>>& common_data_properties() 
                                const
                                noexcept;
    bool operator==(const IndexParameters& other) const noexcept{
        return 
            std::equal(
                    common_.begin(),
                    common_.end(),
                    other.common_.begin(),
                    other.common_.end()) &&
            from_==other.from_ &&
            to_==other.to_ &&
            tdiff_==other.tdiff_ &&
            forecast_preference_==other.forecast_preference_ &&
            level_==other.level_ &&
            top_==other.top_ &&
            bottom_==other.bottom_ &&
            left_==other.left_ &&
            right_==other.right_ &&
            grid_type_==other.grid_type_;
    }
};
using IndexParameters_t = std::variant<std::monostate,
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>;
}

namespace serialization{
    using namespace network;
    template<bool NETWORK_ORDER>
    struct Serialize<NETWORK_ORDER,network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(const type& msg, std::vector<char>& buf) const noexcept{
            return serialize<NETWORK_ORDER>(msg,buf,msg.common_,msg.from_,
                msg.to_,msg.tdiff_,msg.forecast_preference_,msg.level_,
                msg.top_,msg.bottom_,msg.left_,msg.right_,msg.grid_type_);
        }
    };

    template<bool NETWORK_ORDER>
    struct Deserialize<NETWORK_ORDER,network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        SerializationEC operator()(type& msg, StreamSerializer& buf) const noexcept{
            return deserialize<NETWORK_ORDER>(msg,buf,msg.common_,msg.from_,
                msg.to_,msg.tdiff_,msg.forecast_preference_,msg.level_,
                msg.top_,msg.bottom_,msg.left_,msg.right_,msg.grid_type_);
        }
    };

    template<>
    struct Serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        size_t operator()(const type& msg) const noexcept{
            return serial_size(msg.common_,msg.from_,
                msg.to_,msg.tdiff_,msg.forecast_preference_,
                msg.level_,msg.top_,msg.bottom_,msg.left_,
                msg.right_,msg.grid_type_);
        }
    };

    template<>
    struct Min_serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return min_serial_size<decltype(type::common_),
            decltype(type::from_),decltype(type::to_),decltype(type::tdiff_),
            decltype(type::forecast_preference_),decltype(type::level_),
            decltype(type::top_),decltype(type::bottom_),
            decltype(type::left_),decltype(type::right_),
            decltype(type::grid_type_)>();
        }();
    };

    template<>
    struct Max_serial_size<network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>>{
        using type = network::IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>;
        static constexpr size_t value = []() ->size_t
        {
            return max_serial_size<decltype(type::common_),
            decltype(type::from_),decltype(type::to_),decltype(type::tdiff_),
            decltype(type::forecast_preference_),decltype(type::level_),
            decltype(type::top_),decltype(type::bottom_),
            decltype(type::left_),decltype(type::right_),
            decltype(type::grid_type_)>();
        }();
    };
}