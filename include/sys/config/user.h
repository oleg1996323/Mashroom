#pragma once
#include <string>
#include <string_view>
#include <unordered_set>
#include "types/time_interval.h"
#include <expected>
#include "sys/outputdatafileformats.h"
#include "types/time_interval.h"
#include "base_config.h"

namespace user{
    struct Settings{
        /**
         * @brief time-period between index updates
         */
        fs::path output_files_root_dir = "~/Mashroom_output/";
        DateTimeDiff index_update_ti=[](){
            std::error_code err;
            return DateTimeDiff(err,years(0),
                                    months(0),
                                    days(7),
                                    hours(0),
                                    minutes(0),
                                    std::chrono::seconds(0));}();
        /**
         * @brief time-period between Mashroom updates
         */
        DateTimeDiff mashroom_update_ti=[](){
            std::error_code err;
            return DateTimeDiff(err,years(0),
                                    months(0),
                                    days(7),
                                    hours(0),
                                    minutes(0),
                                    std::chrono::seconds(0));}();

        OutputDataFileFormats default_format = OutputDataFileFormats::BIN_F|
                                                OutputDataFileFormats::ARCHIVED;
    };

    class Config:public config::detail::BaseConfig<user::Settings>{
        public:
        Config()=default;
    };
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