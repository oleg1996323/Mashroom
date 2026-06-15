#include "web/client/message/application/detail/index/gribv1.h"

namespace network{

    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    IndexParameters(IndexParameters&& other) noexcept:
        common_(std::move(other.common_)),
        from_(std::move(other.from_)),
        to_(std::move(other.to_)),
        tdiff_(std::move(other.tdiff_)),
        forecast_preference_(std::move(other.forecast_preference_)),
        level_(std::move(other.level_)),
        top_(std::move(other.top_)),
        bottom_(std::move(other.bottom_)),
        left_(std::move(other.left_)),
        right_(std::move(other.right_)),
        grid_type_(std::move(other.grid_type_)){}
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    IndexParameters(const IndexParameters& other) noexcept:
        common_(other.common_),
        from_(other.from_),
        to_(other.to_),
        tdiff_(other.tdiff_),
        forecast_preference_(other.forecast_preference_),
        level_(other.level_),
        top_(other.top_),
        bottom_(other.bottom_),
        left_(other.left_),
        right_(other.right_),
        grid_type_(other.grid_type_){}
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    operator=(IndexParameters&& other) noexcept{
        if(this!=&other){
            common_=std::move(other.common_);
            from_=std::move(other.from_);
            to_=std::move(other.to_);
            tdiff_=std::move(other.tdiff_);
            forecast_preference_=std::move(other.forecast_preference_);
            level_=std::move(other.level_);
            top_=std::move(other.top_);
            bottom_=std::move(other.bottom_);
            left_=std::move(other.left_);
            right_=std::move(other.right_);
            grid_type_=std::move(other.grid_type_);
        }
        return *this;
    }
    
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    operator=(const IndexParameters& other) noexcept{
        if(this!=&other){
            common_=other.common_;
            from_=other.from_;
            to_=other.to_;
            tdiff_=other.tdiff_;
            forecast_preference_=other.forecast_preference_;
            level_=other.level_;
            top_=other.top_;
            bottom_=other.bottom_;
            left_=other.left_;
            right_=other.right_;
            grid_type_=other.grid_type_;
        }
        return *this;
    }
    
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    grid_type(RepresentationType rep) noexcept{
        grid_type_.emplace(rep);
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    from(utc_tp_t<std::chrono::seconds> from) noexcept{
        from_.emplace(from);
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    to(utc_tp_t<std::chrono::seconds> to) noexcept{
        to_.emplace(to);
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    time_diff(DateTimeDiff diff) noexcept{
        tdiff_.emplace(diff);
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    position_rect(
            std::optional<Lat> top,
            std::optional<Lat> bottom,
            std::optional<Lon> left,
            std::optional<Lon> right) noexcept
    {
        top_=top;
        bottom_=bottom;
        left_=left;
        right_=right;
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    forecast_preference(
            TimeForecast fcst,
            TimeForecast::COMPARISION_TYPE comp_type) noexcept
    {
        forecast_preference_.emplace(fcst,comp_type);
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    level_preference(
            Level lvl,
            Level::COMPARISION_TYPE comp_type) noexcept
    {
        level_.emplace(lvl,comp_type);
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    top(const Lat& pos) noexcept
    {
        top_ = pos;
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    bottom(const Lat& pos) noexcept
    {
        bottom_ = pos;
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    left(const Lon& pos) noexcept
    {
        left_ = pos;
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    right(const Lon& pos) noexcept
    {
        right_ = pos;
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    common_data_properties(
        const std::unordered_set<
            CommonDataProperties<Data_t::TIME_SERIES,
                                Data_f::GRIB_v1>>& cmn)
                                noexcept
    {
        common_=cmn;
        return *this;
    }
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    add_common_data_property(
        CommonDataProperties<Data_t::TIME_SERIES,
                                Data_f::GRIB_v1> cmn)
                                noexcept
    {
        common_.insert(cmn);
        return *this;
    }
    const std::optional<RepresentationType>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    grid_type() const noexcept
    {
        return grid_type_;
    }
    const std::optional<utc_tp_t<std::chrono::seconds>>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    from() const noexcept
    {
        return from_;
    }
    const std::optional<utc_tp_t<std::chrono::seconds>>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    to() const noexcept
    {
        return to_;
    }
    const std::optional<DateTimeDiff>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    time_diff() const noexcept
    {
        return tdiff_;
    }
    const std::optional<Lon>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    left() const noexcept
    {
        return left_;
    }
    const std::optional<Lon>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    right() const noexcept
    {
        return right_;
    }
    const std::optional<Lat>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    top() const noexcept
    {
        return top_;
    }
    const std::optional<Lat>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    bottom() const noexcept
    {
        return bottom_;
    }
    const std::optional<std::pair<TimeForecast,
            TimeForecast::COMPARISION_TYPE>>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    forecast_preference() const noexcept
    {
        return forecast_preference_;
    }
    const std::optional<std::pair<Level,Level::COMPARISION_TYPE>>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    level_preference() const noexcept
    {
        return level_;
    }
    const std::unordered_set<
        CommonDataProperties<
            Data_t::TIME_SERIES,
            Data_f::GRIB_v1>>& 
    IndexParameters<Data_t::TIME_SERIES,Data_f::GRIB_v1>::
    common_data_properties() 
                                const
                                noexcept
    {
        return common_;
    }
}