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
#include "Intersection.h"
#include "BasicGeometry.h"
#include "Point.h"

namespace geometry{

template<uint64_t DIMENSIONS,typename VALUE_T= long double>
class BasicPolyline:public BasicGeometry<false,DIMENSIONS>{
    std::vector<BasicPoint<DIMENSIONS,VALUE_T>> vertices_;
    mutable double length_ = 0.;
    mutable bool length_defined_=false;
    using id = uint64_t;
public:
    BasicPolyline() = default;
    BasicPolyline(std::initializer_list<Point<DIMENSIONS>> il):
        vertices_(il.begin(),il.end()){}

    BasicPolyline(const BasicPolyline& other):
        vertices_(other.vertices_),
        length_(other.length_),
        length_defined_(other.length_defined_){}
    BasicPolyline(BasicPolyline&& other):
        vertices_(std::move(other.vertices_)),
        length_(other.length_),
        length_defined_(other.length_defined_){}

    BasicPolyline& operator=(const BasicPolyline& other)
    {
        if(this!=&other){
            vertices_ = other.vertices_;
            length_ = other.length_;
            length_defined_ = other.length_defined_;
        }
        return *this;
    }
    BasicPolyline& operator=(BasicPolyline&& other)
    {
        if(this!=&other){
            vertices_ = std::move(other.vertices_);
            length_ = other.length_;
            length_defined_ = other.length_defined_;
        }
        return *this;
    }
    public:
    double length(/*mode (cartesian/geographic)*/) const noexcept{
        if(length_defined_)
            return length_;
        else{
            long double result = 0;
            for(uint64_t i = 0;i<vertices_.size()-1;++i)
                result+=exec<Distance,DIMENSIONS>(
                        vertices_[i+1],vertices_[i]);
            length_ = result;
            length_defined_ = true;
            return length_;
        }
    }
    template<typename OTHER_VALUE_T>
    void append(Point<DIMENSIONS,OTHER_VALUE_T> coord) noexcept{
        if(!vertices_.empty()){
            if(vertices_.back()!=coord &&
                vertices_.front()!=coord)
                length_defined_ = false;
        }
        else length_defined_ = false;
        vertices_.emplace_back(std::move(coord));
    }

    template<typename OTHER_VALUE_T>
    void insert(uint64_t coord_id,Point<DIMENSIONS,OTHER_VALUE_T> coord) noexcept{
        if(coord_id>=vertices_.size())
            append(coord);
        else{
            if(vertices_[coord_id+1]==coord || (coord_id>0 && vertices_[coord_id-1]==coord))
                length_defined_ = false;
            vertices_.emplace(vertices_.begin()+coord_id,std::move(coord));
        }
    }
    void erase(uint32_t coord_id) noexcept{
        if(vertices_.empty())
            return;
        uint64_t erased_id = coord_id>=vertices_.size()?vertices_.size()-1:coord_id;
        Point<DIMENSIONS> coord = vertices_[erased_id];
        vertices_.erase(vertices_.begin()+erased_id);
        length_defined_ = false;
    }
    size_t number_vertices() const noexcept{
        return vertices_.size();
    }
    size_t number_segments() const noexcept{
        return vertices_.size()<2?0:vertices_.size()-1;
    }
    const std::vector<Point<DIMENSIONS>>& vertices() const noexcept{
        return vertices_;
    }
    
};

template<uint64_t DIMENSIONS,typename VALUE_T = long double>
using Polyline = BasicPolyline<DIMENSIONS,VALUE_T>;

template<uint64_t DIMENSIONS,typename VALUE_T>
void print(const Polyline<DIMENSIONS,VALUE_T>& pg);
template<uint64_t DIMENSIONS,typename VALUE_T>
void print(const std::vector<BasicPolyline<DIMENSIONS,VALUE_T>>& pg);
template<uint64_t DIMENSIONS>
void print(const Intersection<DIMENSIONS>& is);
template<uint64_t DIMENSIONS>
void print(const std::vector<Intersection<DIMENSIONS>>& iss);

template<uint64_t DIMENSIONS,typename VALUE_T>
void print(const BasicPolyline<DIMENSIONS,VALUE_T>& pl){
    std::cout<<'[';
    for(uint64_t first=0,second=1;first<pl.number_segments();++first,++second){
        std::cout<<"(";print(pl.vertices().at(first));std::cout<<",";print(pl.vertices().at(second));std::cout<<")";
        if(second<pl.number_segments())
            std::cout<<",";
    }
    std::cout<<"]"<<std::endl;
}

template<uint64_t DIMENSIONS,typename VALUE_T>
void print(const std::vector<BasicPolyline<DIMENSIONS,VALUE_T>>& pls){
    for(const BasicPolyline<DIMENSIONS,VALUE_T>& pl:pls)
        print(pl);
}

template<uint64_t DIMENSIONS>
void print(const Intersection<DIMENSIONS>& is){
    std::cout<<"intersection: point("<<is.point().lon_<<","<<is.point().lat_<<") "<< \
        "first ID: "<<is.first_elem_id()<<"; "<<"second ID: "<<is.second_elem_id()<<std::endl;
}

template<uint64_t DIMENSIONS>
void print(const std::vector<Intersection<DIMENSIONS>>& iss){
    for(const Intersection<DIMENSIONS>& is:iss)
        print(is);
}
}
#include "equivalent.h"
#include "cut.h"
#include "intersections.h"
#include "intersects.h"

namespace geometry{
template<typename VALUE1_T, typename VALUE2_T>
    std::vector<BasicPolyline<2,std::common_type_t<VALUE1_T,VALUE2_T>>> cut
    (   const BasicPolyline<2,VALUE1_T>& cutted,
        const BasicPolyline<2,VALUE2_T>& cutting,
        bool left_forward)
    {
        using value_result_t = std::common_type_t<VALUE1_T,VALUE2_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        std::vector<BasicPolyline<2,max_prec_t>> result;
        std::vector<Intersection<2>> turns = exec<Intersections,2>(cutted,cutting);
        bool in = false;

        auto intersection_openning = [left_forward,&cutted,&cutting](const Intersection<2>& is){
            const Point<2,max_prec_t>& first_1 = cutted.vertices().at(is.first_elem_id());
            const Point<2,max_prec_t>& second_1 = cutted.vertices().at(is.first_elem_id()+1);
            const Point<2,max_prec_t>& first_2 = cutting.vertices().at(is.second_elem_id());
            const Point<2,max_prec_t>& second_2 = cutting.vertices().at(is.second_elem_id()+1);
            long double side = cross(first_2, second_2, first_1);
            if(side>0)
                return !left_forward;
            else return left_forward; 
        };
        if(!turns.empty())
            in = !intersection_openning(turns.front());
        else return result;
        std::sort(turns.begin(),turns.end(),
        [&cutted](const Intersection<2>& lhs,const Intersection<2>& rhs){
            if(lhs.first_elem_id()==rhs.first_elem_id()){
                const Point<2>& begin_lhs = cutted.vertices().at(lhs.first_elem_id());
                const Point<2>& begin_rhs = cutted.vertices().at(rhs.first_elem_id());
                if(pythagoras2(begin_lhs,lhs.point())==pythagoras2(begin_rhs,rhs.point()))
                    return lhs.second_elem_id()<rhs.second_elem_id();
                else return pythagoras2(begin_lhs,lhs.point())<pythagoras2(begin_rhs,rhs.point());
            }
            else return lhs.first_elem_id()<rhs.first_elem_id();
        });
        print(turns);
        for(uint64_t i=0;i<turns.size();++i){
            BasicPolyline<2,max_prec_t>* current;
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
                    print(cutted.vertices().at(j),"added vertice",true);
                    current->append(cutted.vertices().at(j));
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

    template<typename VALUE1_T, typename VALUE2_T>
    bool equivalent(const BasicPolyline<2,VALUE1_T>& first,
            const BasicPolyline<2,VALUE2_T>& second) noexcept
    {
        using value_result_t = std::common_type_t<VALUE1_T,VALUE2_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        typename std::vector<BasicPoint<2,max_prec_t>>::const_iterator iter = 
            std::find(second.vertices().begin(),second.vertices().end(),
            first.vertices().front());
        if(iter==second.vertices().end())
            return false;
        else{
            uint64_t last = first.number_segments();
            for(uint64_t i = 0, j = std::distance(second.vertices().begin(),iter);
                i<=last;++i,j=j+1)
            {
                uint64_t j_loc = j+1;
                uint64_t i_loc = i+1;
                while(second.vertices().at(j)==second.vertices().at(j_loc)){
                    j=j+1;
                    j_loc = j+1;
                }
                while(first.vertices().at(i)==first.vertices().at(i_loc) && i<last){
                    ++i;
                    ++i_loc;
                }
                if(first.vertices().at(i)!=second.vertices().at(j))
                    return false;
                continue;
            }
            return true;
        }
    }
}

#include "cut.h"
#include "intersections.h"
#include "intersects.h"

namespace geometry{
    template<typename VALUE1_T, typename VALUE2_T>
    std::vector<Intersection<2>> intersections(
        const BasicPolyline<2,VALUE1_T>& pl,
        const BasicLine<2,VALUE2_T>& line)
    {
        using value_result_t = std::common_type_t<VALUE1_T,VALUE2_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        std::vector<Intersection<2>> result;
        if(line.first()==line.second() || pl.vertices().empty())
            return {};
        for(uint64_t i = 0;i<pl.number_vertices();++i){
            BasicLine<2,max_prec_t> current_line(
                pl.vertices().at(i),
                pl.vertices().at((i+1)%pl.number_vertices()));
            if(exec<Intersects,2>(current_line,line)){
                if(auto found = exec<Intersections,2>(current_line,line);
                    !found.empty()){
                    Point<2> point({found.front().point().template get<0>(),
                            found.front().point().template get<1>()});
                    if(!result.empty()){
                        if(result.back().point()!=point)
                            result.push_back(Intersection(point,i,0));
                        else continue;
                    }
                    else result.push_back(Intersection(point,i,0));
                }
                else continue;
            }
            else continue;
        }
        std::sort(result.begin(),result.end(),
            [](const Intersection<2>& lhs,const Intersection<2>& rhs)
        {
            return lhs.first_elem_id()<rhs.first_elem_id();
        });
        return result;
    }

    template<typename VALUE1_T, typename VALUE2_T>
    std::vector<Intersection<2>> intersections(
        const BasicPolyline<2,VALUE1_T>& pl1,
        const BasicPolyline<2,VALUE2_T>& pl2)
    {
        using value_result_t = std::common_type_t<VALUE1_T,VALUE2_T>;
        using max_prec_t = std::common_type_t<value_result_t,long double>;
        std::vector<Intersection<2>> result;
        if(pl2.vertices().empty() || pl1.vertices().empty())
            return {};
        for(uint64_t i = 0;i<pl1.number_segments();++i){
            BasicLine<2,max_prec_t> l1(
                        pl1.vertices().at(i),
                        pl1.vertices().at(i+1));
            for(uint64_t j = 0;j<pl2.number_segments();++j){
                BasicLine<2,max_prec_t> l2(
                        pl2.vertices().at(j),
                        pl2.vertices().at(j+1));
                if(exec<Intersects,2>(l1,l2))
                {
                    if(auto found = exec<Intersections,2>(l1,l2);
                        !found.empty()){
                        BasicPoint<2,max_prec_t> point(found.front().point());
                        if(!result.empty()){
                            if(result.back().point()!=point && result.front().point()!=point)
                                result.push_back(Intersection<2>(point,i,j));
                            else continue;
                        }
                        else result.push_back(Intersection<2>(point,i,j));
                    }
                    else continue;
                }
                else continue;
            }
            
        }
        std::sort(result.begin(),result.end(),
            [](const Intersection<2>& lhs,const Intersection<2>& rhs)
        {
            if(lhs.first_elem_id()==rhs.first_elem_id())
                return lhs.second_elem_id()<rhs.second_elem_id();
            else return lhs.first_elem_id()<rhs.first_elem_id();
        });
        return result;
    }
}