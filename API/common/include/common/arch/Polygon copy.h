#pragma once
#include <vector>
#include <cstdint>
#include "Line.h"
#include "OsterLib/types/coord.h"
#include <future>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <iostream>
#include <boost/geometry.hpp>

class Polygon{
    struct CompareByX{
        bool operator()(const std::pair<const Coord*,const Coord*>& lhs,
            const std::pair<const Coord*,const Coord*>& rhs) const noexcept{
            double minX_lhs = std::min(lhs.first->lon_,lhs.second->lon_);
            double minX_rhs = std::min(rhs.first->lon_,rhs.second->lon_);
            double dx_lhs = lhs.second->lon_-lhs.first->lon_;
            double dx_rhs = rhs.second->lon_-rhs.first->lon_;
            if(dx_lhs>dx_rhs)
                return false;
            else if(dx_lhs==dx_rhs)
                return minX_lhs<=minX_rhs;
            else return true;
        }
        bool operator()(const std::pair<const Coord*,const Coord*>& lhs,
            const Line& rhs) const noexcept{
            double minX_lhs = std::min(lhs.first->lon_,lhs.second->lon_);
            double minX_rhs = std::min(rhs.X1(),rhs.X2());
            double dx_lhs = lhs.second->lon_-lhs.first->lon_;
            double dx_rhs = rhs.X2()-rhs.X1();
            if(dx_lhs>dx_rhs)
                return false;
            else if(dx_lhs==dx_rhs)
                return minX_lhs<=minX_rhs;
            else return true;
        }
        bool operator()(const Line& lhs,
            const std::pair<const Coord*,const Coord*>& rhs) const noexcept{
            double minX_rhs = std::min(rhs.first->lon_,rhs.second->lon_);
            double minX_lhs = std::min(lhs.X1(),lhs.X2());
            double dx_rhs = rhs.second->lon_-rhs.first->lon_;
            double dx_lhs = lhs.X2()-lhs.X1();
            if(dx_lhs>dx_rhs)
                return false;
            else if(dx_lhs==dx_rhs)
                return minX_lhs<=minX_rhs;
            else return true;
        }
    };
    struct CompareByY{
        bool operator()(const std::pair<const Coord*,const Coord*>& lhs,
                const std::pair<const Coord*,const Coord*>& rhs) const noexcept{
            double minY_lhs = std::min(lhs.first->lat_,lhs.second->lat_);
            double minY_rhs = std::min(rhs.first->lat_,rhs.second->lat_);
            double dy_lhs = std::abs(lhs.first->lat_-lhs.second->lat_);
            double dy_rhs = std::abs(rhs.first->lat_-rhs.second->lat_);
            if(dy_lhs>dy_rhs)
                return false;
            else if(dy_lhs==dy_rhs)
                return minY_lhs<=minY_rhs;
            else return true;
        }
    };

    std::vector<Coord> vertices_;
    std::vector<uint64_t> minX_;
    std::vector<uint64_t> maxX_;
    std::vector<uint64_t> minY_;
    std::vector<uint64_t> maxY_;
    static uint64_t vertices_per_thread_;

    void update_minX(double X,uint64_t new_pos) noexcept{
        if(!minX_.empty()){
            auto X_loc = vertices_[minX_.front()].lon_;
            if(X_loc>X){
                minX_.clear();
                minX_.push_back(new_pos);
            }
            else if(X_loc==X)
                minX_.push_back(new_pos);
        }
        else minX_.push_back(new_pos);
    }
    void update_minY(double Y,uint64_t new_pos) noexcept{
        if(!minY_.empty()){
            if(vertices_[minY_.front()].lat_>Y){
                minY_.clear();
                minY_.push_back(new_pos);
            }
            else if(vertices_[minY_.front()].lat_==Y)
                minY_.push_back(new_pos);
        }
        else minY_.push_back(new_pos);
    }
    void update_maxX(double X,uint64_t new_pos) noexcept{
        if(!maxX_.empty()){
            if(vertices_[maxX_.front()].lon_<X){
                maxX_.clear();
                maxX_.push_back(new_pos);
            }
            else if(vertices_[maxX_.front()].lon_==X)
                maxX_.push_back(new_pos);
        }
        else maxX_.push_back(new_pos);
    }
    void update_maxY(double Y,uint64_t new_pos) noexcept{
        if(!maxY_.empty()){
            if(vertices_[maxY_.front()].lat_<Y){
                maxY_.clear();
                maxY_.push_back(new_pos);
            }
            else if(vertices_[maxY_.front()].lat_==Y)
                maxY_.push_back(new_pos);
        }
        else maxY_.push_back(new_pos);
    }

    static bool vertices_comparator(
            const Coord& lhs_first,const Coord& lhs_second, 
            const Coord& rhs_first,const Coord& rhs_second) noexcept{
        // std::cout<<"lhs:("<<lhs_first.lon_<<","<<lhs_first.lat_<<")"<<\
        //     " - ("<<lhs_second.lon_<<","<<lhs_second.lat_<<")"<<std::endl;
        // std::cout<<"rhs:("<<rhs_first.lon_<<","<<rhs_first.lat_<<")"<<\
        //     " - ("<<rhs_second.lon_<<","<<rhs_second.lat_<<")"<<std::endl;
        // double minX_lhs = std::min(lhs_first.lon_,lhs_second.lon_);
        // double minX_rhs = std::min(rhs_first.lon_,rhs_second.lon_);
        // double minY_lhs = std::min(lhs_first.lat_,lhs_second.lat_);
        // double minY_rhs = std::min(rhs_first.lat_,rhs_second.lat_);
        // double maxX_lhs = std::max(lhs_first.lon_,lhs_second.lon_);
        // double maxX_rhs = std::max(rhs_first.lon_,rhs_second.lon_);
        // double maxY_lhs = std::max(lhs_first.lat_,lhs_second.lat_);
        // double maxY_rhs = std::max(rhs_first.lat_,rhs_second.lat_);
        // double dx_lhs = std::abs(lhs_first.lon_-lhs_second.lon_);
        // double dx_rhs = std::abs(rhs_first.lon_-rhs_second.lon_);
        // double dy_lhs = std::abs(lhs_first.lat_-lhs_second.lat_);
        // double dy_rhs = std::abs(rhs_first.lat_-rhs_second.lat_);
        double defX_lhs = (std::max(lhs_first.lon_,lhs_second.lon_)-
                            std::min(lhs_first.lon_,lhs_second.lon_))*
                            lhs_second.lon_-lhs_first.lon_;
        double defX_rhs = (std::max(rhs_first.lon_,rhs_second.lon_)-
                            std::min(rhs_first.lon_,rhs_second.lon_))*
                            rhs_second.lon_-rhs_first.lon_;
        double defY_lhs = -(std::max(lhs_first.lat_,lhs_second.lat_)-
                            std::min(lhs_first.lat_,lhs_second.lat_))*
                            lhs_second.lat_-lhs_first.lat_;
        double defY_rhs = -(std::max(rhs_first.lat_,rhs_second.lat_)-
                            std::min(rhs_first.lat_,rhs_second.lat_))*
                            rhs_second.lat_-rhs_first.lat_;
        return defX_lhs+defY_lhs<defX_rhs+defY_rhs;
    }

    std::vector<std::pair<Coord,std::pair<const Coord*,const Coord*>>> line_intersections(const Line& cut_line) const noexcept{
        std::vector<std::pair<Coord,std::pair<const Coord*,const Coord*>>> intersections;
        for(uint64_t first = 0;first<vertices_.size();++first){
            uint64_t second = (first+1)%vertices_.size();first<vertices_.size();
            std::pair<const Coord*,const Coord*> line = std::make_pair(
                            &vertices_[first],
                            &vertices_[second]);
            if(auto intersection = 
                cut_line.intersection(
                    Line(   vertices_[first].lon_,
                            vertices_[first].lat_,
                            vertices_[second].lon_,
                            vertices_[second].lat_));
                intersection.has_value())
                intersections.push_back({Coord{.lat_=intersection->second,.lon_=intersection->first},line});
        }
        uint64_t offset = std::min(minX_.front(),minY_.front());
        std::sort(intersections.begin(),intersections.end(),
            [this,offset](const decltype(intersections)::value_type& lhs, 
                const decltype(intersections)::value_type& rhs){
            uint64_t norm_lhs = (lhs.second.first-vertices_.data()+offset)%vertices_.size();
            uint64_t norm_rhs = (rhs.second.first-vertices_.data()+offset)%vertices_.size();
            return norm_lhs<norm_rhs;
        });
        return intersections;
    }

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
    bool operator==(const Polygon& other) const noexcept{
        if(vertices_.size()!=other.vertices_.size())
            return false;
        if(minX_.empty()){
            if(other.minX_.empty())
                return true;
            else return false;
        }
        else{
            for(uint64_t start_this = minX_.front(),start_other = other.minX_.front();
                start_this!=minX_.front();start_this=(start_this+1)%other.vertices_.size()){
                if(vertices_[start_this]!=other.vertices_[start_other])
                    return false;
                }
            return true;
        }
    }
    double area() const noexcept{
        // for(auto& [lat,lon]:vertices_)
            //реализовать через трапеции
    }
    void append(Coord coord) noexcept{
        update_minX(coord.lon_,vertices_.size());
        update_minY(coord.lat_,vertices_.size());
        update_maxX(coord.lon_,vertices_.size());
        update_maxY(coord.lat_,vertices_.size());
        vertices_.push_back(std::move(coord));
    }
    void insert(uint32_t coord_id,Coord coord) noexcept{
        if(coord_id>vertices_.size()){
            append(coord);
        }
        else{
            update_minX(coord.lon_,coord_id);
            update_minY(coord.lat_,coord_id);
            update_maxX(coord.lon_,coord_id);
            update_maxY(coord.lat_,coord_id);
            vertices_.insert(vertices_.begin()+coord_id,std::move(coord));
        }
    }
    void erase(uint32_t coord_id) noexcept{
        /* @todo update minmaxXY
        */
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

    //O(n) difficulty
    bool point_inside(const Coord& point,
            uint32_t max_threads = std::thread::hardware_concurrency()) const noexcept
    {
        auto process = [point](std::span<const Coord> subsequence) noexcept
        {
            uint64_t count = 0;
            for(uint64_t i=0;i<subsequence.size();++i){
                if(double k = Line::k(
                    subsequence[(i+1)%subsequence.size()].lon_,
                    subsequence[(i+1)%subsequence.size()].lat_,
                    subsequence[i].lon_,
                    subsequence[i].lat_);std::isfinite(k))
                {
                    if(std::abs(k)<std::numeric_limits<double>::epsilon())
                        continue;
                    else if(k>0)
                        if(Line::straight_not_above_the_point(
                            subsequence[i].lon_,
                            subsequence[i].lat_,
                            k,
                            point.lon_,
                            point.lat_
                        ))
                            ++count;
                    else{
                        if(Line::straight_not_under_the_point(
                            subsequence[i].lon_,
                            subsequence[i].lat_,
                            k,
                            point.lon_,
                            point.lat_
                        ))
                            ++count;
                    }
                }
                else ++count;
            }
            return count;
        };
        uint32_t threads = vertices_.size()/vertices_per_thread_ + 
            vertices_.size()%vertices_per_thread_>vertices_per_thread_/2;
        uint64_t vertices_processed = vertices_per_thread_;
        threads = std::max(threads,max_threads);
        vertices_processed = std::max(vertices_.size()/threads,vertices_processed);
        if(threads>1){
            std::vector<std::future<uint64_t>> processes;
            processes.reserve(threads);
            for(uint32_t i=0;i<vertices_.size();i+=vertices_processed)
                processes.push_back(std::async(std::launch::async,
                        process,
                        std::span<const Coord>(vertices_.begin()+i,vertices_.begin()+vertices_processed)));
            return (std::accumulate(processes.begin(),processes.end(),0,
            [](uint64_t sum,std::future<uint64_t>& process_result)->uint64_t
            {
                return sum+process_result.get();
            })%2)==1;
        }
        else return (process(std::span(vertices_))%2)==1;
    }
    Polygon intersection(const Polygon& other) const noexcept{
        auto init_set = [](const std::vector<Coord>& vertices){
            std::vector<Line> result;
            result.reserve(vertices.size());
            for(uint64_t i = 0;i<vertices.size();++i){
                const Coord& c1 = vertices[i];
                const Coord& c2 = vertices[(i+1)%vertices.size()];
                result.push_back(
                    Line(c1.lon_,c1.lat_,c2.lon_,c2.lat_));
            }
            std::sort(result.begin(),result.end(),[](const Line& lhs,const Line& rhs){
                return std::min(lhs.X1(),lhs.X2())<std::min(rhs.X1(),rhs.X2());
            });
            return result;
        };
        std::vector<Line> X_other(init_set(other.vertices_));
        std::unordered_set<Line,std::hash<Line>,std::equal_to<Line>> checked_inside_;
        Polygon result;
        for(uint64_t i = 0;i<vertices_.size();++i){
            Line this_line(vertices_[i].lon_,
                    vertices_[i].lat_,
                    vertices_[(i+1)%vertices_.size()].lon_,
                    vertices_[(i+1)%vertices_.size()].lat_);
            auto lower = std::lower_bound(X_other.begin(),X_other.end(),std::min(this_line.X1(),this_line.X2()),
                [](const Line& e, double value) { return std::min(e.X1(),e.X2())<value; });
            auto upper = std::upper_bound(X_other.begin(),X_other.end(),std::max(this_line.X1(),this_line.X2()),
                [](double value,const Line& e) { return value<std::min(e.X1(),e.X2()); });
            auto ordered_lines = std::ranges::subrange(lower,upper);
            for(auto& other_line:ordered_lines){
                if(auto intersection = 
                    this_line.intersection(other_line);
                    intersection.has_value()){
                    result.append(Coord{.lat_=intersection->second,.lon_=intersection->first});
                }
                else{
                    if(!checked_inside_.contains(other_line)){
                        if(Coord point = Coord{.lat_=other_line.Y1(),.lon_=other_line.X1()};
                                                        point_inside(point)){
                            result.append(point);
                            checked_inside_.insert(other_line);
                            continue;
                        }
                    }
                    else continue;
                    if(!checked_inside_.contains(this_line)){
                        if(Coord point = Coord{.lat_=this_line.Y1(),.lon_=this_line.X1()};
                                                        other.point_inside(point)){
                            result.append(point);
                            checked_inside_.insert(this_line);
                            continue;
                        }
                    }
                    else continue;
                }
            }
        }        
        return result;
    }
    std::vector<Polygon> cut(const Line& cut_line,bool erase_left_side) const noexcept{
        std::vector<Polygon> result;
        auto intersections = line_intersections(cut_line);
        if(intersections.size()<2)
            return {};
        bool is_forward = true; //в порядке обхода
        int8_t inc;
        {
            const Coord& coord1 = *intersections[0].second.first;
            const Coord& coord2 = *intersections[0].second.second;
            if(coord1.lon_<coord2.lon_){
                is_forward = true;
                inc = erase_left_side?-1:1;
            }
            else if(std::abs(coord1.lon_-coord2.lon_)<std::numeric_limits<double>::epsilon()){
                if(coord1.lat_<coord2.lat_){
                    is_forward = true;
                    inc = erase_left_side?-1:1;
                }
                else{
                    is_forward = false;
                    inc = erase_left_side?1:-1;
                }
            }
            else {
                is_forward = false;
                inc = erase_left_side?1:-1;
            }
        }
        //определяет начальный и конечный индексы точек пересечения, которые образуют левосторонние и правосторонние полигоны
        std::vector<std::pair<uint64_t,uint64_t>> sided_intersections = 
        [this,is_forward](const decltype(intersections)& isections){
            std::vector<std::pair<uint64_t,uint64_t>> result;
            auto& previous = isections[0];
            result.push_back({0,0});
            uint64_t offset = std::min(minX_.front(),minY_.front());
            for(uint64_t i=1;i<isections.size();++i){
                if(isections[i].first.lon_>previous->lon_){
                    previous = &isections[i].first;
                    result.push_back({i,i});
                }
                else if(isections[i].first.lon_==previous->lon_){
                    if(std::abs(isections[i].first.lon_-previous->lon_)<
                        std::numeric_limits<double>::epsilon() &&
                        isections[i].first.lat_>previous->lat_)
                    {
                        previous = &isections[i].first;
                        result.push_back({i,i});
                    }
                    else ++result.back().second;
                }
                else ++result.back().second;
            }
            return result;
        }(intersections);
        for(uint64_t id_side1=0;id_side1<sided_intersections.size();++id_side1){
            Polygon new_pg;
            for(uint64_t id = sided_intersections[id_side1].first;
                    id<=sided_intersections[id_side1].second;
                    ++id){
                uint64_t next_id = (id+1+sided_intersections[id_side1].second)%sided_intersections[id_side1].second;
                new_pg.append(Coord{.lat_=intersections[id].first.lat_,.lon_=intersections[id].first.lon_});
                if(id%2==0)
                    continue;
                else{
                    uint64_t vertice_id;
                    const Coord* expected;
                    if(is_forward){
                        if(erase_left_side){
                            vertice_id = intersections[id].second.first-vertices_.data();
                            expected = intersections[next_id].second.first;
                        }
                        else{
                            vertice_id = intersections[id].second.second-vertices_.data();
                            expected = intersections[next_id].second.second;
                        }
                    }
                    else{
                        if(erase_left_side){
                            vertice_id = intersections[id].second.first-vertices_.data();
                            expected = intersections[next_id].second.second;
                        }
                        else{
                            vertice_id = intersections[id].second.second-vertices_.data();
                            expected = intersections[next_id].second.first;
                        }
                    }

                    for(;;){
                        if(&vertices_[vertice_id]!=expected){
                            new_pg.append(vertices_[vertice_id]);
                            vertice_id=(vertice_id+inc+vertices_.size())%vertices_.size();
                        }
                        else break;
                    }
                }
            }
            result.push_back(new_pg);
        }
        for(uint64_t id_side2=1;id_side2<sided_intersections.size();id_side2+=2){
            
        }
        for(auto& intersection_ids:sided_intersections){
            
        }
        return result;
    }
};