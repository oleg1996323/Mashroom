#pragma once
#include <memory>
#include "OsterLib/types/coord.h"
#include "common/geometry/Polygon.h"
#include "common/projection/AbstractProjection.h"

namespace projection{
    class ProjectionArea {
        std::unique_ptr<projection::AbstractProjection> proj_;
        boost::geometry::model::multi_polygon<boost::geometry::model::polygon<Coord>> clip_bounds_;
    public:
        ProjectionArea() = default;
        virtual bool bounded() const noexcept{
            return !bounds().empty();
        }
        virtual bool contains(const Coord& point) const noexcept{
            namespace geom = boost::geometry;
            return geom::within(clip_bounds_,point);
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
        // Вычисление пересечения с другой областью (возвращает новую область)
        ProjectionArea intersection(const ProjectionArea& other) const noexcept{
            namespace geom = boost::geometry;
            ProjectionArea result;
            geom::intersection(bounds(),other.bounds(),result.clip_bounds_);
            return result;
        }
        // Вычисление пересечения с другой областью (возвращает новую область)
        ProjectionArea& intersect(const ProjectionArea& other) noexcept{
            namespace geom = boost::geometry;
            geom::model::multi_polygon<geom::model::polygon<Coord>> tmp_intersection;
            geom::intersection(bounds(),other.bounds(),tmp_intersection);
            clip_bounds_.swap(tmp_intersection);
            return *this;
        }

        ProjectionArea& cut(const boost::geometry::model::linestring<Coord>& line, bool left_hand) noexcept{
            namespace geom = boost::geometry;
            geom::model::multi_linestring<geom::model::linestring<Coord>> intersect_lines;
            geom::intersection(bounds(),line,intersect_lines);
            if(intersect_lines.size()%2==1)
                return *this;
            for(auto& intersect_lines)
            return *this;
        }

        const decltype(clip_bounds_)& bounds() const noexcept{
            return clip_bounds_;
        }

        // Сериализация/десериализация (опционально)
        // ...
    };
}