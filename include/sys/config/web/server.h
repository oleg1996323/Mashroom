#pragma once
#include <string>
#include <unordered_set>
#include "boost_functional/json.h"
#include "network/serversettings.h"
#include "sys/config/base_config.h"

namespace network::server{

class Config:public config::detail::BaseConfig<network::server::Settings>{
    std::unordered_set<std::string> black_list_;
    std::unordered_set<std::string> white_list_; //@todo save credentials
    public:
    using black_list_t = decltype(black_list_);
    using white_list_t = decltype(white_list_);
    Config() = default;
    const std::unordered_set<std::string>& black_list() const noexcept{
        return black_list_;
    }
    const std::unordered_set<std::string>& white_list() const noexcept{
        return white_list_;
    }
    bool push_to_black_list(const std::string& host) noexcept;
    bool push_to_white_list(const std::string& host) noexcept;
    bool remove_from_white_list(const std::string& host) noexcept;
    bool remove_from_black_list(const std::string& host) noexcept;
    void print_black_list(std::ostream& stream) const noexcept;
    void print_white_list(std::ostream& stream) const noexcept;
};

Settings default_config();
}

template<>
boost::json::value to_json(const network::server::Config& val);

template<>
std::expected<network::server::Config,std::exception> from_json(const boost::json::value& val);