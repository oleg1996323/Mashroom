#include "common/geometry/Polyline.h"

std::vector<Polyline> Polyline::cut_impl(
    const Polyline& other,
    bool left_forward
    ) const noexcept{
    std::vector<Polyline> result;
    std::vector<Intersection> turns = polyline_intersections(other);
    bool starts_by_intersection = false; //отсеченный отрезок начинается на первой точке пересечения
    bool in = false;
    //
    if(!turns.empty()){
        const Coord& first_1 = vertices_[turns.front().first_elem_id()];
        const Coord& second_1 = vertices_[turns.front().first_elem_id()+1];
        const Coord& first_2 = vertices_[turns.front().second_elem_id()];
        const Coord& second_2 = vertices_[turns.front().second_elem_id()+1];
        long double side = ::cross(first_2, second_2, first_1);
        if(side>0)
            starts_by_intersection=!left_forward;
        else
            starts_by_intersection=left_forward;
    }
    else return result;
    std::sort(turns.begin(),turns.end(),
    [this](const Intersection& lhs,const Intersection& rhs){
        if(lhs.first_elem_id()==rhs.first_elem_id()){
            const Coord& begin_lhs = vertices_[lhs.first_elem_id()];
            const Coord& begin_rhs = vertices_[rhs.first_elem_id()];
            if(pythagoras2(begin_lhs,lhs.point())==pythagoras2(begin_rhs,rhs.point()))
                return lhs.second_elem_id()<rhs.second_elem_id();
            else return pythagoras2(begin_lhs,lhs.point())<pythagoras2(begin_rhs,rhs.point());
        }
        else return lhs.first_elem_id()<rhs.first_elem_id();
    });
    Polyline current;
    if(!starts_by_intersection){ //начинается с начальной части полилинии до пересечения
        for(uint64_t i=0;i<=turns.front().first_elem_id();++i)
            current.append(vertices_[turns[i].first_elem_id()]);
        current.append(turns.front().point());
        result.push_back(std::move(current));
    }
    else {
        current.append(turns.front().point());
        in = true;
    }
    for(uint64_t i=0,j=1;j<turns.size();++i,++j){
        if(turns[j].first_elem_id()==turns[i].first_elem_id()){
            if(in){
                current.append(turns[j].point());
                result.push_back(std::move(current));
                in = false;
            }
            else{
                const Coord& first_1 = other.vertices_[turns[i].second_elem_id()];
                const Coord& second_1 = other.vertices_[turns[i].second_elem_id()+1];
                const Coord& first_2 = other.vertices_[turns[j].second_elem_id()];
                const Coord& second_2 = other.vertices_[turns[j].second_elem_id()+1];
                if((cross(first_1,second_1,second_2)>0)==
                    (cross(first_2,second_2,second_1)>0)){
                    if(!current.vertices_.empty())
                        current.vertices_.back() = turns[j].point();
                    else result.back().vertices_.back() = turns[j].point();
                }
                else{
                    current.append(turns[j].point());
                    in = true;
                }
            }
        }
        else{
            if(in){
                for(uint64_t m=i;m<turns.size() && m<turns[j].first_elem_id();++m)
                    current.append(vertices_[turns[m].first_elem_id()]);
                current.append(turns[j].point());
                result.push_back(std::move(current));
                in = false;
            }
            else{
                current.append(turns[j].point());
                in = true;
            }
        }
    }
    return result;
}