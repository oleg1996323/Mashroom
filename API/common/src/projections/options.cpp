#include "common/projections/options.h"

namespace projection{
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
    CommonOptions& CommonOptions::set_projection_maker(
            std::function<std::unique_ptr<AbstractProjection>(
                const std::unordered_map<std::string,attribute_t>&)>&& func) noexcept
    {
        proj_func_ = std::forward<decltype(func)>(func);
    }
    size_t CommonOptions::hash() const noexcept{
        return boost::hash<boost::json::object>()(attributes_);
    }
    bool CommonOptions::operator==(const CommonOptions& other) const noexcept{
        return attributes_==other.attributes_;
    }
}
