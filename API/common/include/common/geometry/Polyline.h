#pragma once
#include <cstdlib>
#include <cstdint>
#include <vector>
#include "Line.h"
#include "OsterLib/types/coord.h"
#include <future>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <iostream>
#include "algorithms.h"
#include <limits>
#include <span>
#include <ranges>
#include <future>
#include <algorithm>
#include <numeric>
#include <functional>
#include <thread>
#include "intersection.h"
#include "Polyline_view.h"
#include "BasicGeometry.h"

template<typename POINT_T>
class Polyline:public BasicPolyline<POINT_T>{
    std::vector<POINT_T> vertices_;
    mutable double length_ = 0.;
    mutable bool length_defined_=false;

    using id = uint64_t;

    std::vector<Intersection<POINT_T>> line_intersections(const Line& cut_line) const noexcept{
        std::vector<Intersection<POINT_T>> result;
        if(cut_line.first()==cut_line.second() || vertices_.empty())
            return {};
        for(uint64_t i = 0;i<vertices_.size();++i){
            auto& first = vertices_[i];
            auto& second = vertices_[(i+1)%vertices_.size()];
            if(::intersects(first,second,
                cut_line.first(),cut_line.second())){
                if(auto found = cut_line.intersection(first.lon_,first.lat_,second.lon_,second.lat_);
                    found.has_value()){
                    POINT_T coord{  .lat_ = found.value().second,
                            .lon_ = found.value().first};
                    if(!result.empty()){
                        if(result.back().point()!=coord)
                            result.push_back(Intersection(coord,i,0));
                        else continue;
                    }
                    else result.push_back(Intersection(coord,i,0));
                }
                else continue;
            }
            else continue;
        }
        std::sort(result.begin(),result.end(),
            [](const Intersection<POINT_T>& lhs,const Intersection<POINT_T>& rhs)
        {
            return lhs.first_elem_id()<rhs.first_elem_id();
        });
        return result;
    }
    std::vector<Intersection<POINT_T>> polyline_intersections(const Polyline& other) const noexcept{
        std::vector<Intersection<POINT_T>> result;
        if(other.vertices_.empty() || vertices_.empty())
            return {};
        for(Line::id i = 0;i<vertices_.size()-1;++i){
            auto& first_1 = vertices_[i];
            auto& second_1 = vertices_[i+1];
            Line this_line(first_1,second_1);
            for(Line::id j = 0;j<other.vertices_.size()-1;++j){
                auto& first_2 = other.vertices_[j];
                auto& second_2 = other.vertices_[j+1];
                if(::intersects(first_1,second_1,
                    first_2,second_2))
                {
                    if(auto found = this_line.intersection(Line(first_2.lon_,first_2.lat_,second_2.lon_,second_2.lat_));
                        found.has_value()){
                        POINT_T coord{  .lat_ = found.value().second,
                                .lon_ = found.value().first};
                        if(!result.empty()){
                            if(result.back().point()!=coord && result.front().point()!=coord)
                                result.push_back(Intersection(coord,i,j));
                            else continue;
                        }
                        else result.push_back(Intersection(coord,i,j));
                    }
                    else continue;
                }
                else continue;
            }
            
        }
        std::sort(result.begin(),result.end(),
            [](const Intersection<POINT_T>& lhs,const Intersection<POINT_T>& rhs)
        {
            if(lhs.first_elem_id()==rhs.first_elem_id())
                return lhs.second_elem_id()<rhs.second_elem_id();
            else return lhs.first_elem_id()<rhs.first_elem_id();
        });
        return result;
    }
    uint64_t define_turn_id(
        const std::vector<Intersection<POINT_T>>& turns,
        const Intersection<POINT_T>& ti) const noexcept{
        uint64_t result = std::lower_bound(turns.begin(),turns.end(),ti,
            [](const Intersection<POINT_T>& t,const Intersection<POINT_T>& turn)
        {
            if(t.first_elem_id()==turn.first_elem_id())
                return t.second_elem_id()<turn.second_elem_id();
            else return t.first_elem_id()<turn.first_elem_id();
        }) - turns.begin();
        return result;
    }

    //@todo проверить касается ли секущая вершины
    std::vector<Polyline> cut_impl(
    const Polyline& segment,
    bool left_forward
    ) const noexcept;
    public:
    Polyline()=default;
    Polyline(const Polyline& other):
        vertices_(other.vertices_),
        length_(other.length_){}
    Polyline(Polyline&& other) noexcept:
        vertices_(std::move(other.vertices_)),
        length_(other.length_){}
    Polyline& operator=(const Polyline& other){
        if(this!=&other){
            vertices_=other.vertices_;
            length_ = other.length_;
        }
        return *this;
    }
    Polyline& operator=(Polyline&& other){
        if(this!=&other){
            vertices_=std::move(other.vertices_);
            length_ = other.length_;
        }
        return *this;
    }
    bool operator==(const Polyline& other) const noexcept{
        return this == &other ||
            (vertices_.size()==other.vertices_.size() &&
            length()==other.length());
    }
    std::optional<double> length(/*mode (cartesian/geographic)*/) const noexcept{
        if(length_defined_)
            return length_;
        else{
            long double result = 0;
            for(uint64_t i = 0;i<vertices_.size()-1;++i){
                long double dX = vertices_[i+1].lon_-
                        vertices_[i].lon_;
                long double dY = vertices_[i+1].lat_-
                        vertices_[i].lat_;
                result+=std::sqrtl(dX*dX+dY*dY);
            }
            length_ = result;
            length_defined_ = true;
            return length_;
        }
    }

    void append(POINT_T coord) noexcept{
        if(!vertices_.empty()){
            if(vertices_.back()!=coord &&
                vertices_.front()!=coord)
                length_defined_ = false;
        }
        else length_defined_ = false;
        vertices_.push_back(std::move(coord));
    }
    void insert(uint64_t coord_id,POINT_T coord) noexcept{
        if(coord_id>=vertices_.size())
            append(coord);
        else{
            if(vertices_[coord_id+1]==coord || (coord_id>0 && vertices_[coord_id-1]==coord))
                length_defined_ = false;
            vertices_.insert(vertices_.begin()+coord_id,std::move(coord));
        }
    }
    void erase(uint32_t coord_id) noexcept{
        if(vertices_.empty())
            return;
        uint64_t erased_id = coord_id>=vertices_.size()?vertices_.size()-1:coord_id;
        POINT_T coord = vertices_[erased_id];
        vertices_.erase(vertices_.begin()+erased_id);
        length_defined_ = false;
    }
    size_t number_vertices() const noexcept{
        return vertices_.size();
    }
    size_t number_segments() const noexcept{
        return vertices_.size()-1;
    }
    const std::vector<POINT_T>& vertices() const noexcept{
        return vertices_;
    }
    std::vector<Polyline> intersection(const Polyline& other) const noexcept;
    static std::vector<Polyline> cut(
        const Line& line,
        const Polyline& pg,
        bool left_forward
        ) noexcept;
    std::vector<Polyline> cut(
        const Line& line,
        bool left_forward
        ) const noexcept;
    static std::vector<Polyline> cut(
        const Polyline& cutted_polyline,
        const Polyline& cutting_polyline,
        bool left_forward
        ) noexcept
    {
        return cutted_polyline.cut(cutting_polyline,left_forward);
    }
    std::vector<Polyline> cut(
        const Polyline& other,
        bool left_forward
        ) const noexcept
    {
        return cut_impl(other,left_forward);
    }
    bool equivalent(const Polyline& other,bool multithread) const noexcept{
        std::vector<POINT_T>::const_iterator iter = 
            std::find(other.vertices_.begin(),other.vertices_.end(),
            vertices_.front());
        if(iter==other.vertices_.end())
            return false;
        else{
            uint64_t last = vertices_.size()-1;
            for(uint64_t i = 0, j = std::distance(other.vertices_.begin(),iter);
                i<=last;++i,j=j+1)
            {
                uint64_t j_loc = j+1;
                uint64_t i_loc = i+1;
                while(other.vertices_[j]==other.vertices_[j_loc]){
                    j=j+1;
                    j_loc = j+1;
                }
                while(vertices_[i]==vertices_[i_loc] && i<last){
                    ++i;
                    ++i_loc;
                }
                if(vertices_[i]!=other.vertices_[j])
                    return false;
                continue;
            }
            return true;
        }
    }
};
template<typename POINT_T>
void print(const Polyline<POINT_T>& pg);
template<typename POINT_T>
void print(const std::vector<Polyline<POINT_T>>& pg);
template<typename POINT_T>
void print(const Intersection<POINT_T>& is);
template<typename POINT_T>
void print(const std::vector<Intersection<POINT_T>>& iss);

#include "common/geometry/Polyline.h"

template<typename POINT_T>
void print(const Polyline<POINT_T>& pl){
    std::cout<<'[';
    for(uint64_t first=0,second=1;first<pl.number_segments();++first,++second){
        std::cout<<"(";print(pl.vertices().at(first));std::cout<<",";print(pl.vertices().at(second));std::cout<<")";
        if(second<pl.number_segments())
            std::cout<<",";
    }
    std::cout<<"]"<<std::endl;
}

template<typename POINT_T>
void print(const std::vector<Polyline<POINT_T>>& pls){
    for(const Polyline& pl:pls)
        print(pl);
}

template<typename POINT_T>
void print(const Intersection<POINT_T>& is){
    std::cout<<"intersection: point("<<is.point().lon_<<","<<is.point().lat_<<") "<< \
        "first ID: "<<is.first_elem_id()<<"; "<<"second ID: "<<is.second_elem_id()<<std::endl;
}

template<typename POINT_T>
void print(const std::vector<Intersection<POINT_T>>& iss){
    for(const Intersection<POINT_T>& is:iss)
        print(is);
}

template<typename POINT_T>
std::vector<Polyline<POINT_T>> Polyline<POINT_T>::cut_impl(
    const Polyline& other,
    bool left_forward
    ) const noexcept{
    std::vector<Polyline<POINT_T>> result;
    std::vector<Intersection<POINT_T>> turns = polyline_intersections(other);
    bool in = false;
    //

    auto intersection_openning = [this,left_forward,&other](const Intersection<POINT_T>& is){
        const POINT_T& first_1 = vertices_[is.first_elem_id()];
        const POINT_T& second_1 = vertices_[is.first_elem_id()+1];
        const POINT_T& first_2 = other.vertices_[is.second_elem_id()];
        const POINT_T& second_2 = other.vertices_[is.second_elem_id()+1];
        long double side = ::cross(first_2, second_2, first_1);
        if(side>0)
            return !left_forward;
        else return left_forward; 
    };
    if(!turns.empty())
        in = !intersection_openning(turns.front());
    else return result;
    std::sort(turns.begin(),turns.end(),
    [this](const Intersection<POINT_T>& lhs,const Intersection<POINT_T>& rhs){
        if(lhs.first_elem_id()==rhs.first_elem_id()){
            const POINT_T& begin_lhs = vertices_[lhs.first_elem_id()];
            const POINT_T& begin_rhs = vertices_[rhs.first_elem_id()];
            if(pythagoras2(begin_lhs,lhs.point())==pythagoras2(begin_rhs,rhs.point()))
                return lhs.second_elem_id()<rhs.second_elem_id();
            else return pythagoras2(begin_lhs,lhs.point())<pythagoras2(begin_rhs,rhs.point());
        }
        else return lhs.first_elem_id()<rhs.first_elem_id();
    });
    print(turns);
    for(uint64_t i=0;i<turns.size();++i){
        Polyline* current;
        if(in){
            current = result.empty()?&result.emplace_back():&result.back();
            while(i+1<turns.size() && intersection_openning(turns[i+1])){
                ++i;
            }
            if(i>=turns.size())
                break;
            uint64_t start_vertice = (i==0)?0:turns[i-1].first_elem_id()+1;
            uint64_t last_vertice = 0;
            last_vertice = turns[i].first_elem_id()+1;
                                    
            for(uint64_t j=start_vertice;j<last_vertice;++j){
                print(vertices_[j],"added vertice",true);
                current->append(vertices_[j]);
            }
            print(turns[i].point(),"added vertice",true);
            current->append(turns[i].point());
            in=false;
            std::cout<<"Made polyline"<<std::endl;
            print(result.back());
        }
        else {
            while(i+1<turns.size() && intersection_openning(turns[i+1])){
                ++i;
            }
            if(i>=turns.size())
                break;
            if(!result.empty() && result.back().vertices().back()==turns[i].point()){
                current = &result.back();
            }
            else{
                current = &result.emplace_back();
                print(turns[i].point(),"added vertice",true);
                current->append(turns[i].point());
            }
            in=true;
        }

    }
    return result;
}


