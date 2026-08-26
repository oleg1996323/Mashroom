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

namespace grid{
    namespace detail{
        std::error_code register_projection(std::string_view name,std::function<std::unique_ptr<AbstractProjection>()> function) noexcept;
        std::unique_ptr<AbstractProjection> make_reference_projection(std::string_view name) noexcept;
    }
    
    class CommonOptions{
        public:
        using attribute_t = boost::json::value;
        private:
        boost::json::object attributes_;
        public:
        CommonOptions(
                std::string_view name,
                std::string_view REF,
                std::chrono::sys_seconds timepoint,
                int64_t API_val,
                int64_t ID){
            add_attribute("name",name)
            .add_attribute("timepoint",timepoint.time_since_epoch())
            .add_attribute("API",API_val)
            .add_attribute("ID",ID);
        }
        CommonOptions(const CommonOptions& other);
        CommonOptions(CommonOptions&& other) noexcept;
        CommonOptions& operator=(const CommonOptions& other);
        CommonOptions& operator=(CommonOptions&& other) noexcept;
        std::string_view name() const noexcept{
            return attribute("name").as_string();
        }
        int64_t API() const noexcept{
            return attribute("API").as_int64();
        }
        int64_t ID() const noexcept{
            return attribute("ID").as_int64();
        }
        std::chrono::sys_seconds timepoint() const noexcept{
            return std::chrono::sys_seconds(std::chrono::seconds(attribute("timepoint").as_int64()));
        }
        bool contains(std::string_view name) const noexcept;
        const attribute_t& attribute(std::string_view attr_name)const noexcept;
        template<typename T>
        CommonOptions& add_attribute(std::string_view attr_name,T value) 
                noexcept(std::is_move_assignable_v<T>)
        {
            if(attr_name.empty() || attributes_.contains(attr_name))
                return *this;
            else{
                if(attr_name=="timepoint"){
                    if constexpr(std::is_convertible_v<T,std::string_view>){
                        std::chrono::sys_seconds tp;
                        std::istringstream iss(value.c_str());
                        iss>>std::chrono::parse("%Y-%m-%dT%H:%M:%SZ",tp);
                        if(iss.fail())
                            return *this;
                        else options->set_attribute("timepoint",tp.time_since_epoch());
                    }
                    else if constexpr(std::is_same_v<T,int64_t>){
                        attributes_[attr_name]=to_json(value);
                    }
                    else static_assert("timepoint may be staticly initialized by string_view-convertible type or by int64_t");
                }
                else attributes_[attr_name]=to_json(value);
            }
            return *this;
        }
        const boost::json::object& attributes() const noexcept{
            return attributes_;
        }
        CommonOptions& set_projection_maker(
            std::function<std::unique_ptr<AbstractProjection>(
                const boost::json::object&)>&& func) noexcept;
        size_t hash() const noexcept;
        bool operator==(const CommonOptions& other) const noexcept;
        std::unique_ptr<AbstractProjection> make_universal() const noexcept;
        virtual const CommonOptions& instance() const noexcept{
            return *this;
        }
    };

    CommonOptions::CommonOptions(const CommonOptions& other):
    attributes_(other.attributes_){}

    CommonOptions::CommonOptions(CommonOptions&& other) noexcept:
    attributes_(std::move(other.attributes_)){}
    CommonOptions& CommonOptions::operator=(const CommonOptions& other){
        if(this!=&other){
            attributes_ = other.attributes_;
        }
        return *this;
    }
    CommonOptions& CommonOptions::operator=(CommonOptions&& other) noexcept{
        if(this!=&other){
            attributes_.swap(other.attributes_);
        }
        return *this;
    }
    bool CommonOptions::contains(std::string_view name) const noexcept{
        return attributes_.contains(name);
    }
    const CommonOptions::attribute_t& CommonOptions::attribute(
            std::string_view attr_name)const noexcept
    {
        static attribute_t empty;
        if(auto found = attributes_.find(attr_name);found!=attributes_.end())
            return found->value();
        return empty;
    }
    std::unique_ptr<AbstractProjection> CommonOptions::make_universal() const noexcept
    {
        return detail::make_reference_projection(name());
    }
    size_t CommonOptions::hash() const noexcept{
        return boost::hash<boost::json::object>()(attributes_);
    }
    bool CommonOptions::operator==(const CommonOptions& other) const noexcept{
        return attributes_==other.attributes_;
    }
}

template<>
struct std::hash<grid::CommonOptions>{
    size_t operator()(const grid::CommonOptions& opt) const noexcept{
        return opt.hash();
    }
};

template<>
struct std::hash<std::unique_ptr<grid::CommonOptions>>{
    using is_transparent = std::true_type;
    size_t operator()(const std::unique_ptr<grid::CommonOptions>& opt) const noexcept{
        if(opt)
            return opt->hash();
        else return 0;
    }
};

template<>
struct std::equal_to<std::unique_ptr<grid::CommonOptions>>{
    using is_transparent = std::true_type;
    bool operator()(const std::unique_ptr<grid::CommonOptions>& lhs,
            const std::unique_ptr<grid::CommonOptions>& rhs) const{
        if((lhs && rhs))
            return *lhs==*rhs;
        else if(!lhs && !rhs)
            return true;
        else return false;
    }
};

#if defined(GRIB1API) || defined(GRIB2API) || defined(GRIB3API)
namespace grid::api::grib{
    using RepresentationType = uint8_t;
    using Organization = uint8_t;

#ifdef GRIB1API
namespace v1{
    std::shared_ptr<grid::CommonOptions> get_grid_options(
            Organization center,
            RepresentationType id,
            bool rotated,
            bool stretched) noexcept;
}
#endif
#ifdef GRIB2API
namespace v2{
    std::shared_ptr<grid::CommonOptions> get_grid_options(
            Organization center,
            RepresentationType id,
            bool rotated,
            bool stretched) noexcept;
}
#endif
#ifdef GRIB3API
namespace v3{
    std::shared_ptr<grid::CommonOptions> get_grid_options(
            Organization center,
            RepresentationType id,
            bool rotated,
            bool stretched) noexcept;
}
#endif
}
#endif

namespace grid{
std::expected<std::shared_ptr<grid::CommonOptions>,
    std::error_code> add_grid_options(
        const boost::json::object& options) noexcept;

std::expected<std::shared_ptr<grid::CommonOptions>,
    std::error_code> add_grid_options(
    std::shared_ptr<grid::CommonOptions>) noexcept;

std::error_code add_projections_options_from_json(const std::filesystem::path& file);

std::shared_ptr<grid::CommonOptions> get_grid_options(
        const boost::json::object& attributes) noexcept;
std::shared_ptr<CommonOptions> get_grid_options(
        std::string_view name,
        std::chrono::sys_seconds last_timepoint,
        int64_t id,
        int64_t api) noexcept;

std::error_code add_options_from_json(const std::filesystem::path& file) noexcept;
}