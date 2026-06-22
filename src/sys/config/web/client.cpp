#include "config/web/client.h"

template<>
boost::json::value to_json(const network::client::Config& val){
    using namespace boost;
    return val.base_to_json().as_object();
}

template<>
std::expected<network::client::Config,std::exception> 
        from_json(const boost::json::value& val){
    network::client::Config result;
    if(val.is_object()){
        auto& c = val.as_object();
        if(auto base_res = result.base_from_json(val);
            base_res.has_value())
            return std::unexpected(base_res.value());
    }
    else if(val.is_null())
        return result;
    else return std::unexpected(std::invalid_argument("not object-type in client-config"));
    return result;
}