#include "web/server/message/application/detail/index.h"

template<>
std::expected<network::IndexResult,std::exception> 
    from_json<network::IndexResult>(const boost::json::value& val){
    
}

template<>
boost::json::value to_json(const network::IndexResult& val){
    boost::json::value result;
    auto visit = [&result](const auto& data_val){
        if constexpr(std::is_same_v<std::monostate,std::decay_t<decltype(data_val)>>)
            return;
        else{
            result = to_json(data_val);
        }
    };
    if(std::holds_alternative<std::monostate>(val)){
        auto& obj = result.emplace_object();
        obj["data type"]="none";
        obj["data format"]="none";
    }
    else
        std::visit(visit,val);
    return result;
}
