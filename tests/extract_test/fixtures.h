#include <gtest/gtest.h>
#include "proc/extract/extracted_value.h"
#include <random>

class Grib1Fixture: public testing::Test{
    ExtractedData data_;
    SearchProperties props_;
    TimePeriod time_p;
    protected:
    Grib1Fixture():time_p(years(0),months(1),days(0),hours(0),minutes(0),seconds(0)){}
    virtual void SetUp() override{
        props_.center_ = Organization::ECMWF;
        props_.fcst_unit_ = TimeFrame::HOUR;
        props_.from_date_ = sys_days(1990y/1/1d);
        props_.to_date_ = sys_days(1990y/1/2d);
        props_.grid_type_ = RepresentationType::LAT_LON_GRID_EQUIDIST_CYLINDR;
        props_.position_ = Coord{.lat_=45.f,.lon_=45.f};
        props_.parameters_.insert(SearchParamTableVersion{.param_=246,.t_ver_=228});
        props_.parameters_.insert(SearchParamTableVersion{.param_=247,.t_ver_=228});
        ExtractedValues<Data_t::TIME_SERIES,Data_f::GRIB_v1> values;
        Grib1CommonDataProperties common1;
        common1.center_ = Organization::ECMWF;
        common1.fcst_unit_ = TimeFrame::HOUR;
        common1.table_version_ = 228;
        common1.parameter_ = 246;
        auto& val_collection = values[common1];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::weibull_distribution<ExtractedValue<Data_t::TIME_SERIES,Data_f::GRIB_v1>::value_t> dist(2.f,7.f);
        for(utc_tp time = sys_days(1990y/1/1d);time<sys_days(1990y/1/2d);time+=hours(1))
            val_collection.emplace_back(time,dist(gen));
        Grib1CommonDataProperties common2;
        common2.center_ = Organization::ECMWF;
        common2.fcst_unit_ = TimeFrame::HOUR;
        common2.table_version_ = 228;
        common2.parameter_ = 247;
        val_collection = values[common2];
        for(utc_tp time = sys_days(1990y/1/1d);time<sys_days(1990y/1/2d);time+=hours(1))
            val_collection.emplace_back(time,dist(gen));
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