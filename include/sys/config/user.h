#pragma once
#include <string>
#include <string_view>
#include <unordered_set>
#include "types/time_interval.h"
#include <expected>

namespace user{
    struct Settings{
        std::string name_;
        /**
         * @brief time-interval between capitalize updates
         */
        TimeOffset capitalize_update_ti_;
        /**
         * @brief time-interval between Mashroom updates
         */
        TimeOffset mashroom_update_ti_;
    };

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