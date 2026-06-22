#pragma once
#include <unordered_map>
#include <string>
#include <type_traits>
#include <iostream>
#include <functional>

namespace config::detail{

template<typename SETTINGS>
class BaseConfig{
    std::unordered_map<std::string, 
        SETTINGS> configs_;
    
    std::string current_name_;
    protected:
    virtual std::unordered_map<std::string, 
        SETTINGS>& configurations() noexcept{
        return configs_;
    }
    public:
    using name_t = typename decltype(configs_)::key_type;
    using settings_t = typename decltype(configs_)::mapped_type;
    virtual ~BaseConfig() = default;
    BaseConfig():
        current_name_("default"){
            configs_["default"];
        }
    BaseConfig(BaseConfig&& other) noexcept:
    configs_(std::move(other.configs_)),
    current_name_(std::move(other.current_name_)){}
    BaseConfig& operator=(BaseConfig&& other) noexcept{
        if(this!=&other){
            configs_=std::move(other.configs_);
            current_name_=std::move(other.current_name_);
        }
        return *this;
    }
    bool add(const std::string& name, SETTINGS&& settings) noexcept{
        if(!configurations().contains(name)){
            configurations().insert({name,std::forward<SETTINGS>(settings)});
            return true;
        }
        else return false;
    }
    bool add_from_file(const std::string& name, const fs::path& filename) noexcept;
    bool modify_from_file(const std::string& name, const fs::path& filename) noexcept;
    bool remove(const std::string& name) noexcept{
        if(configurations().contains(name)){
            configurations().erase(name);
            return true;
        }
        else return false;
    }
    bool modify(const std::string& name,SETTINGS&& settings) noexcept{
        if(auto found = configurations().find(name);found!=configurations().end()){
            found->second=std::forward<SETTINGS>(settings);
            return true;
        }
        else return false;
    }
    bool contains(std::string_view name) const noexcept{
        return configurations().contains(name);
    }
    bool contains(const std::string& name) const noexcept{
        return configurations().contains(name);
    }
    virtual void print(const std::string& name, std::ostream& stream) const{
        if(configs_.contains(name)){
            stream<<name<<":\n\t"<<to_json(configs_.at(name))<<std::endl;
        }
        else std::cout<<"Not found"<<std::endl;
    }
    void print_all(std::ostream& stream) const{
        for(const auto& [name,settings]:configs_)
            print(name,stream);
    }
    void print_current(std::ostream& stream) const{
        print(current_name_,stream);
    }
    virtual const std::unordered_map<std::string, 
        SETTINGS>& configurations() const noexcept{
        return const_cast<BaseConfig*>(this)->configurations();
    }
    const std::string& current_name() const noexcept{
        return current_name_;
    }
    const SETTINGS& current_settings() const noexcept{
        return configs_.at(current_name_);
    }
    bool set_current(const std::string& name) noexcept{
        if(current_name_!=name){
            if(auto tmp = configs_.find(name);tmp!=configs_.end()){
                current_name_ = name;
                return true;
            }
            else return false;
        }
        else return false;
    }
    void reset() noexcept{
        current_name_="default";
        configs_.clear();
        configs_["default"];
    }

    template<String NAME>
    SETTINGS* get_config(NAME&& name) noexcept{
        if(auto found = configurations().find(name);found!=configurations().end())
            return &found->second;
        else return nullptr;
    }
    template<String NAME>
    const SETTINGS* get_config(NAME&& name) const noexcept{
        if(auto found = configurations().find(name);found!=configurations().end())
            return &found->second;
        else return nullptr;
    }

    boost::json::value base_to_json() const{
        using namespace boost;
        json::object map;
        boost::json::array configurations;
        for(auto& [name,settings]:this->configurations()){
            json::object tmp;
            tmp["name"] = name;
            tmp["settings"] = to_json(settings);
            configurations.push_back(tmp);
        }
        map["configurations"] = std::move(configurations);
        map["current"] = current_name_;
        return map;
    }

    std::optional<std::exception>
            base_from_json(const boost::json::value& val){
        auto& c = val.as_object();
        if(c.contains("configurations")){
            if(c.at("configurations").is_array()){
                auto& arr = c.at("configurations").as_array();
                for(auto& arr_val:arr){
                    if(arr_val.is_object()){
                        auto& pair = arr_val.as_object();
                        if(pair.contains("name") && pair.contains("settings"))
                            if(auto name_tmp = from_json<name_t>(pair.at("name"));
                                name_tmp.has_value())
                            {
                                if(name_tmp.value().empty())
                                    return std::invalid_argument("\"name\" empty");
                                if(auto sets_tmp = from_json<settings_t>(pair.at("settings"));
                                    sets_tmp.has_value())
                                    add(name_tmp.value(),std::move(sets_tmp.value()));                                
                            }
                            else if(pair.at("name").is_null())
                                return std::invalid_argument("\"name\" empty");
                            else return std::invalid_argument("\"name\" not string");
                    }
                }
            }
        }
        if(c.contains("current") && c.at("current").is_string()){
            auto current = c.at("current").as_string();
            if(contains(std::string(current.data(),current.size())))
                set_current(current.c_str());
            else set_current("default");
        }
        else set_current("default");
        return std::nullopt;
    }
};

template<typename SETTINGS>
bool BaseConfig<SETTINGS>::add_from_file(const std::string& name, const fs::path& filename) noexcept{
    if(!name.empty() && 
        !configs_.contains(name) &&
        fs::exists(filename) &&
        fs::is_regular_file(filename)){
        auto parsed = parse_json_from_file(filename);
        if(parsed.has_value()){
            auto parsed_settings = from_json<SETTINGS>(parsed.value());
            if(parsed_settings.has_value()){
                add(name,std::move(parsed_settings.value()));
                return true;
            }
            else return false;
        }
        else return false;
    }
    else return false;
}

template<typename SETTINGS>
bool BaseConfig<SETTINGS>::modify_from_file(const std::string& name, const fs::path& filename) noexcept{
    if(!name.empty() && 
        configs_.contains(name) &&
        fs::exists(filename) &&
        fs::is_regular_file(filename)){
        auto parsed = parse_json_from_file(filename);
        if(parsed.has_value()){
            auto parsed_settings = from_json<SETTINGS>(parsed.value());
            if(parsed_settings.has_value()){
                modify(name,std::move(parsed_settings.value()));
                return true;
            }
            else return false;
        }
        else return false;
    }
    else return false;
}
}

#include "boost_functional/json.h"

