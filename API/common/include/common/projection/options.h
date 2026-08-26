#pragma once
#include <memory>
#include <string_view>
#include <string>
#include <cstdint>
#include <functional>
#include <chrono>

namespace projection{
    class AbstractProjection;
    std::unique_ptr<AbstractProjection> make_projection
        (std::string_view name) noexcept;

    template<typename Projection_t>
    std::unique_ptr<Projection_t> make_projection
        (std::string_view name) noexcept
    {
        static_assert(std::is_base_of_v<AbstractProjection,Projection_t>);
        if(auto proj = make_projection(name);
                proj.get()!=nullptr)
        {
            if(auto* casted = dynamic_cast<Projection_t*>(proj.get())){
                return std::unique_ptr<Projection_t>(proj.release());
            }
            else return {};
        }
        else return {};
    }

    std::error_code register_projection(
            std::string name,
            std::function<
                std::unique_ptr<AbstractProjection>()> function) noexcept;
}