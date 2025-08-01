#pragma once
#include <string>
#include <string_view>
#include <unordered_set>
#include "types/time_interval.h"
#include <expected>
#include "sys/outputdatafileformats.h"

namespace user{
    struct Settings{
        std::string name_;
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
    
    static Settings default_config() noexcept{
        return Settings{.name_="default",
                        .index_update_ti_=TimePeriod(years(0),
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

    using SettingsHash = std::hash<Settings>;
    using SettingsEqual = std::equal_to<Settings>;
}

template<>
struct std::hash<user::Settings>{
    using is_transparent = std::true_type;
    size_t operator()(const user::Settings& sets) const{
        return std::hash<std::string>()(sets.name_);
    }
    size_t operator()(const std::string& name) const{
        return std::hash<std::string>()(name);
    }
    size_t operator()(std::string_view name) const{
        return std::hash<std::string_view>()(name);
    }
};
template<>
struct std::equal_to<user::Settings>{
    using is_transparent = std::true_type;
    bool operator()(const user::Settings& sets, const std::string& name) const{
        return sets.name_==name;
    }
    bool operator()(const std::string& name,const user::Settings& sets) const{
        return sets.name_==name;
    }
    bool operator()(const user::Settings& sets, std::string_view name) const{
        return sets.name_==name;
    }
    bool operator()(std::string_view name,const user::Settings& sets) const{
        return sets.name_==name;
    }
    bool operator()(const user::Settings& lhs,const user::Settings& rhs) const{
        return lhs.name_==rhs.name_;
    }
};

#include "boost_functional/json.h"

template<>
boost::json::value to_json(const user::Settings& val);

template<>
std::expected<user::Settings,std::exception> from_json(const boost::json::value& val);