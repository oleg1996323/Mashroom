#pragma once
#include <string>
#include <string_view>
#include <unordered_set>
#include "types/time_interval.h"
#include <expected>
#include "sys/outputdatafileformats.h"
#include "types/time_period.h"
#include "base_config.h"

namespace user{
    struct Settings{
        /**
         * @brief time-period between index updates
         */
        TimePeriod index_update_ti_;
        /**
         * @brief time-period between Mashroom updates
         */
        TimePeriod mashroom_update_ti_;
        OutputDataFileFormats default_format_ = OutputDataFileFormats::BIN_F|
                                                OutputDataFileFormats::ARCHIVED;
    };

    class Config:public BaseConfig<user::Settings>{
        public:
        Config()=default;
        void print(std::ostream& stream) const override final{
            
        }
    };
    
    static Settings default_config() noexcept{
        return Settings{.index_update_ti_=TimePeriod(years(0),
                                                months(0),
                                                days(7),
                                                hours(0),
                                                minutes(0),
                                                std::chrono::seconds(0)),
                        .mashroom_update_ti_=TimePeriod(years(0),
                                                months(0),
                                                days(7),
                                                hours(0),
                                                minutes(0),
                                                std::chrono::seconds(0))};
    }
}

#include "boost_functional/json.h"

template<>
boost::json::value to_json(const user::Config& val);

template<>
std::expected<user::Config,std::exception> 
        from_json(const boost::json::value& val);

template<>
boost::json::value to_json(const user::Settings& val);

template<>
std::expected<user::Settings,std::exception> from_json(const boost::json::value& val);