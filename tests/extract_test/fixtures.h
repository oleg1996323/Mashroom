#include <gtest/gtest.h>
#include "proc/extract/extracted_data.h"
#include "types/time_interval.h"
#include "types/time_period.h"
#include "properties.h"
#include <random>

class Grib1Fixture: public testing::Test{
    ExtractedData data_;
    SearchProperties props_;
    TimePeriod time_p;
    protected:
    Grib1Fixture():time_p(years(0),months(1),days(0),hours(0),minutes(0),std::chrono::seconds(0)){}
    virtual void SetUp() override{
        props_.center_ = Organization::ECMWF;
        props_.fcst_unit_ = TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0});
        props_.from_date_ = sys_days(1990y/1/1d);
        props_.to_date_ = sys_days(1990y/1/2d);
        props_.grid_type_ = RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
        props_.position_ = Coord{.lat_=45.f,.lon_=45.f};
        props_.parameters_.insert(SearchParamTableVersion{.param_=246,.t_ver_=228});
        props_.parameters_.insert(SearchParamTableVersion{.param_=247,.t_ver_=228});
        ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1> values;
        procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1> extract_data_properties1;
        extract_data_properties1.cmn_.center_ = Organization::ECMWF;
        extract_data_properties1.cmn_.table_version_ = 228;
        extract_data_properties1.cmn_.parameter_ = 246;
        extract_data_properties1.add_.fcst_ = TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0});
        grid::GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid1;
        grid1.base_.dx=1;
        grid1.base_.dy=1;
        grid1.base_.nx=50;
        grid1.base_.ny=50;
        grid1.base_.x1=0;
        grid1.base_.x2=50;
        grid1.base_.y1=50;
        grid1.base_.y2=0;
        extract_data_properties1.add_.grid_= grid1;
        extract_data_properties1.add_.level_=Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::weibull_distribution<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>::value_t> dist(2.f,7.f);
        {
            auto& val_collection = values[extract_data_properties1];
            for(utc_tp_t<std::chrono::seconds> time = sys_days(1990y/1/1d);time<sys_days(1990y/1/2d);time+=hours(1))
                val_collection.emplace_back(time,dist(gen));
        }
        procedures::extract::details::ExtractDataProperties<Data_t::TIME_SERIES,Data_f::GRIB_v1> extract_data_properties2;
        extract_data_properties2.cmn_.center_ = Organization::ECMWF;
        extract_data_properties2.cmn_.table_version_ = 228;
        extract_data_properties2.cmn_.parameter_ = 247;
        extract_data_properties2.add_.fcst_ = TimeForecast(TimeFrame::HOUR,TimeRangeIndicator::INIT_REF_TIME,{0},{0});
        extract_data_properties2.add_.level_=Level(LevelsTags::GROUND_OR_WATER_SURFACE,10,0);
        grid::GridDefinition<RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR> grid2;
        grid2.base_.dx=1;
        grid2.base_.dy=1;
        grid2.base_.nx=50;
        grid2.base_.ny=50;
        grid2.base_.x1=0;
        grid2.base_.x2=50;
        grid2.base_.y1=50;
        grid2.base_.y2=0;
        extract_data_properties2.add_.grid_= grid2;
        {
            auto& val_collection = values[extract_data_properties2];
            for(utc_tp_t<std::chrono::seconds> time = sys_days(1990y/1/1d);time<sys_days(1990y/1/2d);time+=hours(1))
                val_collection.emplace_back(time,dist(gen));
        }
        data_ = values;
    }

    virtual void TearDown() override{

    }
    public:
    ExtractedData& data(){
        return data_;
    }
    const SearchProperties& props() const{
        return props_;
    }
    const TimePeriod& time_period() const{
        return time_p;
    }
};