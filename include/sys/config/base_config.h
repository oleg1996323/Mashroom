#pragma once
#include <unordered_map>
#include <string>
#include <type_traits>
#include <iostream>
#include <functional>

template<typename SETTINGS>
class BaseConfig{
    static std::unordered_map<std::string, 
        SETTINGS> configs_;
    
    std::string current_name_;
    std::reference_wrapper<SETTINGS> current_settings_;
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
        current_name_("default"),
        current_settings_(std::ref(configs_.at(current_name_))){}
    template<typename CONFIG_SETS>
    bool add(const std::string& name, CONFIG_SETS&& settings) noexcept{
        static_assert(std::is_same_v<std::decay_t<SETTINGS>,CONFIG_SETS>);
        if(!configurations().contains(name)){
            configurations().insert({name,std::forward<CONFIG_SETS>(settings)});
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
    template<typename CONFIG_SETS>
    bool modify(const std::string& name,CONFIG_SETS&& settings) noexcept{
        if(auto found = configurations().find(name);found!=configurations().end()){
            configurations().erase(found);
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
    virtual void print(std::ostream&) const = 0;
    virtual const std::unordered_map<std::string, 
        SETTINGS>& configurations() const noexcept{
        return const_cast<BaseConfig*>(this)->configurations();
    }
    const std::string& current_name() const noexcept{
        return current_name_;
    }
    const SETTINGS& current_settings() const noexcept{
        return current_settings_.get();
    }
    bool set_current(const std::string& name) noexcept{
        if(current_name_!=name){
            if(auto tmp = configs_.find(name);tmp!=configs_.end()){
                current_settings_=std::ref(*tmp);
                current_name_ = name;
                return true;
            }
            else return false;
        }
        else return false;
    }
    void reset() noexcept{
        current_name_="default";
        SETTINGS new_conf={{"default",configs_.at("default")}};
        configs_.swap(new_conf);
        current_settings_=std::ref(configs_.at(current_name_));
    }

    template<String NAME>
    std::reference_wrapper<SETTINGS> get_config(NAME&& name) noexcept{
        if(auto found = configurations().find(name);found!=configurations().end())
            return std::ref(found.second);
        else std::reference_wrapper<SETTINGS>();
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
            if(parsed_settings.has_value())
                add(name,std::move(parsed_settings.value()));
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
            if(parsed_settings.has_value())
                modify(name,std::move(parsed_settings.value()));
            else return false;
        }
        else return false;
    }
    else return false;
}