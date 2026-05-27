#include "config/web/server.h"
#include "network/definitions.h"

template<>
boost::json::value to_json(const network::server::Config& val){
    using namespace boost;
    json::object map;
    boost::json::array configurations;
    for(auto& [name,settings]:val.configurations()){
        json::object tmp;
        tmp["name"] = name;
        tmp["settings"] = to_json(settings);
        configurations.push_back(tmp);
    }
    map["configurations"] = std::move(configurations);
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
        if(c.contains("configurations")){
            if(c.at("configurations").is_array()){
                auto& arr = c.at("configurations").as_array();
                for(auto& arr_val:arr){
                    if(arr_val.is_object()){
                        auto& pair = arr_val.as_object();
                        if(pair.contains("name") && pair.contains("settings"))
                            if(auto name_tmp = from_json<decltype(result)::name_t>(pair.at("name"));
                                name_tmp.has_value())
                            {
                                if(name_tmp.value().empty())
                                    return std::unexpected(
                                        std::invalid_argument("\"name\" empty"));
                                if(auto sets_tmp = from_json<decltype(result)::settings_t>(pair.at("settings"));
                                    sets_tmp.has_value())
                                    result.add(name_tmp.value(),std::move(sets_tmp.value()));                                
                            }
                            else if(pair.at("name").is_null())
                                return std::unexpected(
                                    std::invalid_argument("\"name\" empty"));
                            else return std::unexpected(
                                    std::invalid_argument("\"name\" not string"));
                    }
                }
            }
        }
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
    void Config::print(std::ostream& stream) const{
        stream<<"Server configurations:"<<std::endl;
        int i = 1;
        for(auto& [name,conf]:configurations()){
            stream<<"name: \""<<name<<"\""<<std::endl;
            stream<<"\thost: \'"<<conf.host_<<"\n"<<
                "\'\tservice: \'"<<conf.service_<<"\n"<<
                "\'\tport: \'"<<conf.port_<<"\n"<<
                "\'\tprotocol: \'"<<network::protocol::to_text(conf.protocol_)<<"\n"<<
                "\'\ttimeout: "<<conf.timeout_seconds_processes_<<" seconds"<<"\n"<<
                "\'\tjobs parallel: "<<conf.num_threads_pool_<<" threads\n"<<"\n"<<
                "\'\tevents in process: "<<conf.number_events_<<" events"<<std::endl;
                    
        }
        stream<<"White list: ";
        {
            auto joined = white_list_ | std::views::join_with(';');
            for (char addr : joined)
                stream << addr;
            stream<<std::endl;
        }
        {
            stream<<"Black list: ";
            auto joined = black_list_ | std::views::join_with(';');
            for (char addr : joined)
                stream << addr;
            stream<<std::endl;
        }
    }
}