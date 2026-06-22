#include "config/web/server.h"
#include "network/definitions.h"

bool network::server::Config::push_to_black_list(const std::string& host) noexcept{
    if(!host.empty() && !black_list_.contains(host)){
        black_list_.insert(host);
        if(white_list_.contains(host))
            white_list_.erase(host);
        return true;
    }
    else return false;
}
bool network::server::Config::push_to_white_list(const std::string& host) noexcept{
    if(!host.empty() && !white_list_.contains(host)){
        white_list_.insert(host);
        if(black_list_.contains(host))
            black_list_.erase(host);
        return true;
    }
    else return false;
}
bool network::server::Config::remove_from_white_list(const std::string& host) noexcept{
    if(!white_list_.contains(host))
        return false;
    white_list_.erase(host);
    return true;
}
bool network::server::Config::remove_from_black_list(const std::string& host) noexcept{
    if(!black_list_.contains(host))
        return false;
    black_list_.erase(host);
    return true;
}
void network::server::Config::print_black_list(std::ostream& stream) const noexcept{
    stream<<'[';
    for(auto& host:black_list_)
        stream<<host<<',';
    if(!black_list_.empty())
        stream.seekp(-1,std::ios_base::end);
    stream<<']'<<std::endl;
}
void network::server::Config::print_white_list(std::ostream& stream) const noexcept{
    stream<<'[';
    for(auto& host:white_list_)
        stream<<host<<',';
    if(!black_list_.empty())
        stream.seekp(-1,std::ios_base::end);
    stream<<']'<<std::endl;
}

template<>
boost::json::value to_json(const network::server::Config& val){
    using namespace boost;
    json::object map = val.base_to_json().as_object();
    map["black list"] = to_json(val.black_list());
    map["white list"] = to_json(val.white_list());
    return map;
}

template<>
std::expected<network::server::Config,std::exception> 
        from_json(const boost::json::value& val){
    network::server::Config result;
    if(val.is_object()){
        auto& c = val.as_object();
        if(auto base_res = result.base_from_json(val);
            base_res.has_value())
            return std::unexpected(base_res.value());
        if(c.contains("black list"))
            if(auto tmp = from_json<network::server::Config::black_list_t>(c.at("black list"));
                tmp.has_value()){
                    for(auto& host:tmp.value())
                        result.push_to_black_list(host);
            }
        if(c.contains("white list"))
            if(auto tmp = from_json<network::server::Config::white_list_t>(c.at("white list"));
                tmp.has_value())
                    for(auto& host:tmp.value())
                        result.push_to_white_list(host);
    }
    else if(val.is_null())
        return result;
    else return std::unexpected(std::invalid_argument("not object-type in client-config"));
    return result;
}

namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::string_view_literals;
namespace network::server{
    constexpr int min_timeout_seconds = 1;
    Settings default_config(){
        Settings config_;
        config_.host_="10.10.10.10";
        config_.service_="0";
        config_.protocol_=Protocol::TCP;
        config_.timeout_seconds_processes_ = 20;
        config_.num_threads_pool_ = std::thread::hardware_concurrency();
        return config_;
    }
}