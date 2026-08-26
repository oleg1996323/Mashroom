#pragma once
#include <memory>
#include "OsterLib/types/coord.h"
#include "common/Polygon.h"
#include "common/projection/AbstractProjection.h"

namespace projection{
    class ProjectionArea {
        std::unique_ptr<projection::AbstractProjection> proj_;
        std::vector<Polygon> clip_bounds_;
    public:
        ProjectionArea() = default;
        // Проверка, является ли область пустой
        virtual bool is_empty() const noexcept{

        }

        // Проверка, содержит ли область точку
        virtual bool contains(const Coord& point) const noexcept{

        }
        virtual bool visible(const Coord& point) const noexcept{

        }
        void set_projection(std::unique_ptr<projection::AbstractProjection> projection){
            proj_ = std::move(projection);
        }
        /// @brief 
        /// @return 
        const projection::AbstractProjection* projection() const noexcept{
            return proj_.get();
        }
        projection::AbstractProjection* projection() noexcept{
            return proj_.get();
        }
        bool projection(std::string_view name) noexcept{
            if(auto made = projection::make_projection(name);!made)
                return false;
            else {
                if(proj_ && proj_->projection()==made->projection())
                    return true;
                else proj_ = std::move(made);
                return true;
            }
        }
        // Вычисление пересечения с другой областью (возвращает новую область или nullptr, если пересечения нет)
        ProjectionArea intersect(const ProjectionArea& other) const noexcept{
            ProjectionArea result;
            
        }

        std::unique_ptr<ProjectionArea> clip(const ProjectionArea& other) const noexcept{

        }

        const std::vector<Polygon>& bounds() const noexcept{
            return clip_bounds_;
        }

        // Сериализация/десериализация (опционально)
        // ...
    };
}