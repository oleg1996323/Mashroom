#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include "common/api_types.h"
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>
#include <utility>
#include <OsterLib/boost_functional/json.h>
#include <iostream>
#include <boost/container_hash/hash.hpp>

class AbstractProjection;

namespace projection{
    
    class CommonOptions{
        public:
        using attribute_t = boost::json::value;
        private:
        boost::json::object attributes_;
        std::function<std::unique_ptr<AbstractProjection>(
                const std::unordered_map<std::string,attribute_t>&)> proj_func_;
        public:
        CommonOptions(std::string_view name,int64_t API_val){
            add_attribute("name",name)
            .add_attribute("API",API_val);
        }
        CommonOptions(const CommonOptions& other);
        CommonOptions(CommonOptions&& other) noexcept;
        CommonOptions& operator=(const CommonOptions& other);
        CommonOptions& operator=(CommonOptions&& other) noexcept;
        int64_t id() const noexcept;
        bool contains(std::string_view name) const noexcept;
        const attribute_t& attribute(std::string_view attr_name)const noexcept;
        template<typename T>
        CommonOptions& add_attribute(std::string_view attr_name,T value) 
                noexcept(std::is_move_assignable_v<T>)
        {
            if(attr_name.empty() || attributes_.contains(attr_name))
                return std::make_error_code(std::errc::invalid_argument);
            else attributes_[attr_name]=to_json(value);
            return {};
        }
        const boost::json::object& attributes() const noexcept{
            return attributes_;
        }
        CommonOptions& set_projection_maker(
            std::function<std::unique_ptr<AbstractProjection>(
                const std::unordered_map<std::string,attribute_t>&)>&& func) noexcept;
        size_t hash() const noexcept;
        bool operator==(const CommonOptions& other) const noexcept;
        template<typename PROJ>
        std::unique_ptr<PROJ> make_projection(
            const std::unordered_map<std::string,attribute_t>& attributes) noexcept{
            return std::unique_ptr<PROJ>(proj_func_(attributes).release());
        }
    };
};

template<>
struct std::hash<projection::CommonOptions>{
    size_t operator()(const projection::CommonOptions& opt) const noexcept{
        return opt.hash();
    }
};

template<>
struct std::hash<std::unique_ptr<projection::CommonOptions>>{
    using is_transparent = std::true_type;
    size_t operator()(const std::unique_ptr<projection::CommonOptions>& opt) const noexcept{
        if(opt)
            return opt->hash();
        else return 0;
    }
};

template<>
struct std::equal_to<std::unique_ptr<projection::CommonOptions>>{
    using is_transparent = std::true_type;
    bool operator()(const std::unique_ptr<projection::CommonOptions>& lhs,
            const std::unique_ptr<projection::CommonOptions>& rhs) const{
        if((lhs && rhs))
            return *lhs==*rhs;
        else if(!lhs && !rhs)
            return true;
        else return false;
    }
};
