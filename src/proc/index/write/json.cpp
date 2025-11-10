#include "proc/index/write/json.h"

template<>
std::expected<std::vector<GribMsgDataInfo>,std::exception> from_json(const boost::json::value& val){
    std::vector<GribMsgDataInfo> result;
    if(auto arr = val.if_array();arr){
        for(auto& msg:*arr)
            if(std::expected<GribMsgDataInfo, std::exception> res_msg = from_json<GribMsgDataInfo>(msg);res_msg.has_value())
                result.emplace_back(res_msg.value());
            else return std::unexpected(std::invalid_argument("invalid JSON struct"));
        return result;
    }
    else
        return std::unexpected(std::invalid_argument("invalid JSON struct"));
}

template<>
boost::json::value to_json(const std::vector<GribMsgDataInfo>& val);