#pragma once
#include <vector>
#include "OsterLib/types/coord.h"

namespace projection{

class Polygon{
    std::vector<Coord> vertices_;
    public:
    Polygon()=default;
    Polygon(const Polygon& other):
        vertices_(other.vertices_){}
    Polygon(Polygon&& other) noexcept:
        vertices_(std::move(other.vertices_)){}
    Polygon& operator=(const Polygon& other){
        if(this!=&other){
            vertices_=other.vertices_;
        }
        return *this;
    }
    Polygon& operator=(Polygon&& other){
        if(this!=&other){
            vertices_=std::move(other.vertices_);
        }
        return *this;
    }
    void append(Coord coord) noexcept{
        vertices_.push_back(std::move(coord));
    }
    void insert(uint32_t coord_id,Coord coord) noexcept{
        if(coord_id>vertices_.size())
            vertices_.push_back(std::move(coord));
        else vertices_.insert(vertices_.begin()+coord_id,std::move(coord));
    }
    void erase(uint32_t coord_id) noexcept{
        if(coord_id>vertices_.size()){
            if(!vertices_.empty())
                vertices_.erase(vertices_.end()-1);
        }
        else vertices_.erase(vertices_.begin()+coord_id);
    }
    size_t number_vertices() const noexcept{
        return vertices_.size();
    }
    const std::vector<Coord>& vertices() const noexcept{
        return vertices_;
    }
};
}