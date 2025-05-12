#pragma once
#include <vector>
#include <string_view>

class AbstractCommandHolder{
    std::vector<std::string_view> commands_;
    public:
    virtual ~AbstractCommandHolder() = default;
    std::string_view add_command(std::string_view str){
        return commands_.emplace_back(str);
    }

};