#pragma once
#include <memory>
#include "OsterLib/types/coord.h"

class ProjectionArea {
public:
    virtual ~ProjectionArea() = default;

    // Проверка, является ли область пустой
    virtual bool isEmpty() const noexcept = 0;

    // Проверка, содержит ли область точку
    virtual bool contains(const Coord& point) const noexcept = 0;

    // Вычисление пересечения с другой областью (возвращает новую область или nullptr, если пересечения нет)
    virtual std::unique_ptr<ProjectionArea> intersect(const ProjectionArea& other) const noexcept = 0;

    // Клонирование
    virtual std::unique_ptr<ProjectionArea> clone() const noexcept = 0;

    // Получение границ в виде прямоугольника (для обратной совместимости)
    virtual std::pair<Lat, Lat> latRange() const noexcept = 0;
    virtual std::pair<Lon, Lon> lonRange() const noexcept = 0;

    // Сериализация/десериализация (опционально)
    // ...
};