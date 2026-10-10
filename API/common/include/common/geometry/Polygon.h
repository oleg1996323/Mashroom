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
#include "algorithms.h"



class Polygon{
    std::vector<Coord> vertices_;
    std::vector<uint64_t> minX_;
    std::vector<uint64_t> maxX_;
    std::vector<uint64_t> minY_;
    std::vector<uint64_t> maxY_;
    mutable double area_ = 0;
    mutable bool area_defined_ = false;
    mutable bool wise_defined_ = false;
    mutable bool valid_defined_ = false;
    mutable bool clockwise_ = false;
    mutable bool is_valid_ = false;

    static uint64_t vertices_per_thread_;

    bool would_be_valid_after_erase(uint64_t i) const noexcept {
        const uint64_t n = vertices_.size();
        if (n <= 4) return false;
        const Coord& prev = vertices_[(i - 1 + n) % n];
        const Coord& next = vertices_[(i + 1) % n];
        const uint64_t segments_sz = n - 1;
        for (uint64_t j = 0; j < segments_sz; ++j) {
            if (j == (i - 1 + n) % n) continue;
            if (j == i) continue;
            if (j == (i + 1) % n) continue;
            if (j == (i - 2 + n) % n) continue;
            if (::intersects(prev, next, vertices_[j], vertices_[(j + 1) % n]))
                return false;
        }
        return true;
    }

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

    bool is_clockwise() const noexcept{
        if(wise_defined_)
            return clockwise_;
        else{
            if(vertices_.size()<3)
                return false;
            else{
                double result = 0.;
                for(uint64_t i = 0,j=1;i<vertices_.size();++i,j=(j+1)%vertices_.size()){
                    const Coord& first = vertices_[i];
                    const Coord& second = vertices_[j];
                    result+=second.lat_*first.lon_-first.lat_*second.lon_;
                }
                double scale =  std::max(std::abs(vertices_[maxY_.front()].lat_),
                                std::abs(vertices_[minY_.front()].lat_))*
                                std::max(std::abs(vertices_[maxX_.front()].lon_),
                                std::abs(vertices_[minX_.front()].lon_));
                if(std::abs(result) < 
                    std::numeric_limits<double>::epsilon()*scale*vertices_.size())
                    return false;
                else if(result>0){
                    wise_defined_ = true;
                    clockwise_ = false;
                    return clockwise_;
                }
                else{
                    wise_defined_ = true;
                    clockwise_ = true;
                    return clockwise_;
                }
            }
        }
    }

    std::vector<std::pair<Coord,Line::id>> line_intersections(const Line& cut_line) const noexcept{
        std::vector<std::pair<Coord,Line::id>> result;
        for(Line::id i = 0;i<vertices_.size();++i){
            auto& first = vertices_[i];
            auto& second = vertices_[(i+1)%vertices_.size()];
            if(::intersects(first,second,
                cut_line.first(),cut_line.second())){
                if(auto found = cut_line.intersection(Line(first.lon_,first.lat_,second.lon_,second.lat_));
                    found.has_value()){
                    Coord coord{  .lat_ = found.value().second,
                            .lon_ = found.value().first};
                    if(!result.empty()){
                        if(result.back().first!=coord && result.front().first!=coord)
                            result.push_back(std::make_pair(coord,i));
                        else continue;
                    }
                    else result.push_back(std::make_pair(coord,i));
                }
                else continue;
            }
            else continue;
        }
        std::sort(result.begin(),result.end(),
            [](const std::pair<Coord,Line::id>& lhs,const std::pair<Coord,Line::id>& rhs)
        {
            return lhs.second<rhs.second;
        });
        return result;
    }
    uint64_t define_turn_id(const std::vector<std::pair<Coord,Line::id>>& turns,const std::pair<Coord,Line::id>& ti){
        uint64_t result = std::lower_bound(turns.begin(),turns.end(),ti,
            [](const std::pair<Coord,Line::id>& t,const std::pair<Coord,Line::id>& turn)
        {
            return t.second<turn.second;
        }) - turns.begin();
        return result;
    }

    //@todo проверить касается ли секущая вершины
    std::vector<Polygon> cut_impl(
    const Line& segment,
    const Polygon& pg,
    bool left_forward,
    bool clockwise
    );
    public:
    Polygon()=default;
    Polygon(const Polygon& other):
        vertices_(other.vertices_),
        wise_defined_(other.wise_defined_),
        area_defined_(other.area_defined_),
        valid_defined_(other.valid_defined_),
        area_(other.area_),
        clockwise_(other.clockwise_),
        is_valid_(other.is_valid_),
        maxX_(other.maxX_),
        maxY_(other.maxY_),
        minX_(other.minX_),
        minY_(other.minY_){}
    Polygon(Polygon&& other) noexcept:
        vertices_(std::move(other.vertices_)),
        wise_defined_(other.wise_defined_),
        area_defined_(other.area_defined_),
        valid_defined_(other.valid_defined_),
        area_(other.area_),
        clockwise_(other.clockwise_),
        is_valid_(other.is_valid_),
        maxX_(std::move(other.maxX_)),
        maxY_(std::move(other.maxY_)),
        minX_(std::move(other.minX_)),
        minY_(std::move(other.minY_)){}
    Polygon& operator=(const Polygon& other){
        if(this!=&other){
            vertices_=other.vertices_;
            wise_defined_ = other.wise_defined_;
            area_defined_ = other.area_defined_;
            valid_defined_ = other.valid_defined_;
            area_ = other.area_;
            clockwise_ = other.clockwise_;
            is_valid_ = other.is_valid_;
            maxX_ = other.maxX_;
            maxY_ = other.maxY_;
            minX_ = other.minX_;
            minY_ = other.minY_;
        }
        return *this;
    }
    Polygon& operator=(Polygon&& other){
        if(this!=&other){
            vertices_=std::move(other.vertices_);
            wise_defined_ = other.wise_defined_;
            area_defined_ = other.area_defined_;
            valid_defined_ = other.valid_defined_;
            area_ = other.area_;
            clockwise_ = other.clockwise_;
            is_valid_ = other.is_valid_;
            maxX_ = std::move(other.maxX_);
            maxY_ = std::move(other.maxY_);
            minX_ = std::move(other.minX_);
            minY_ = std::move(other.minY_);
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
                start_this!=minX_.front();start_this=(start_this+1)%other.vertices_.size())
            {
                if(vertices_[start_this]!=other.vertices_[start_other])
                    return false;
            }
            return true;
        }
    }
    double area(/*mode (cartesian/geographic)*/) const noexcept{
        if(area_defined_)
            return area_;
        else{
            long double result = 0;
            for(uint64_t i = 0,j=i+1;i<vertices_.size();++i,j=(j+1)%vertices_.size())
                result+=vertices_[j].lat_*vertices_[i].lon_-vertices_[j].lon_*vertices_[i].lat_;
            area_ = result/2.;
            area_defined_ = true;
            return area_;
        }
    }
    void correct() noexcept{
        if(vertices_.size()>2){
            if(vertices_.front()!=vertices_.back()){
                vertices_.push_back(vertices_.front());
                is_clockwise();
            }
            else{
                is_clockwise();
            }
        }
    }
    bool is_valid() const noexcept{
        if(valid_defined_)
            return is_valid_;
        else{
            valid_defined_ = true;
            if(vertices_.size()<3 || vertices_.front()!=vertices_.back()){
                is_valid_ = false;
                return is_valid_;
            }
            else{
                //if self-intersections are not allowed
                uint64_t last_segment = vertices_.size()-1;
                while(vertices_[last_segment]==
                    vertices_[(last_segment-1+
                    vertices_.size())%vertices_.size()]&&
                    last_segment>1)
                {
                    --last_segment;
                }
                if(last_segment<2){
                    is_valid_ = false;
                    return is_valid_;
                }
                else --last_segment;
                uint64_t first_segment = 0;
                while(first_segment<last_segment && 
                    vertices_[first_segment]==
                        vertices_[first_segment+1])
                    ++first_segment;
                for(uint64_t i=first_segment;i<=last_segment;++i){
                    Line init_line(vertices_[i],vertices_[i+1]);
                    uint64_t j_last = last_segment;
                    if(i==first_segment)
                        --j_last;
                    for(uint64_t j=i+2;j<=j_last;++j){
                        while(j<=last_segment && vertices_[j]==vertices_[j+1])
                            ++j;
                        if(j>last_segment)
                            break;
                        else{
                            if ((i-j+vertices_.size())%vertices_.size()<2) break;
                            if(init_line.intersects(Line(vertices_[j],vertices_[j+1]))){
                                is_valid_ = false;
                                return is_valid_;
                            }
                            else continue;
                        }
                    }
                }
                is_valid_ = true;
                return is_valid_;
            }
        }
    }
    void close(){
        if(vertices_.size()>2)
            vertices_.push_back(vertices_.front());
    }
    void append(Coord coord) noexcept{
        if(vertices_.size()>2 && vertices_.front()==coord)
            if(vertices_.back() == coord)
                return;
            else close();
        else{
            update_minX(coord.lon_,vertices_.size());
            update_minY(coord.lat_,vertices_.size());
            update_maxX(coord.lon_,vertices_.size());
            update_maxY(coord.lat_,vertices_.size());
            vertices_.push_back(std::move(coord));
        }
    }
    void insert(uint64_t coord_id,Coord coord) noexcept{
        if(coord_id>=vertices_.size()){
            if( vertices_.size()>2 && 
                vertices_.front()==vertices_.back())
                vertices_.pop_back();
            append(coord);
        }
        else{
            if (vertices_[coord_id] == coord) return;
            if (coord_id > 0 && vertices_[coord_id - 1] == coord) return;
            vertices_.insert(vertices_.begin()+coord_id,std::move(coord));
            std::for_each(minX_.begin(),minX_.end(),[coord_id](uint64_t& id){
                if(id>=coord_id)
                    ++id;
            });
            std::for_each(maxX_.begin(),maxX_.end(),[coord_id](uint64_t& id){
                if(id>=coord_id)
                    ++id;
            });
            std::for_each(minY_.begin(),minY_.end(),[coord_id](uint64_t& id){
                if(id>=coord_id)
                    ++id;
            });
            std::for_each(maxY_.begin(),maxY_.end(),[coord_id](uint64_t& id){
                if(id>=coord_id)
                    ++id;
            });
            update_minX(coord.lon_,coord_id);
            update_maxX(coord.lon_,coord_id);
            update_minY(coord.lat_,coord_id);
            update_maxY(coord.lat_,coord_id);
        }
        area_defined_ = false;
        valid_defined_ = false;
        wise_defined_ = false;
    }
    void erase(uint32_t coord_id) noexcept{
        if(vertices_.empty())
            return;
        uint64_t erased_id = coord_id>=vertices_.size()?vertices_.size()-1:coord_id;
        Coord coord = vertices_[erased_id];
        bool erase_maxX = (coord.lon_ == vertices_[maxX_.front()].lon_);
        bool erase_maxY = (coord.lat_ == vertices_[maxY_.front()].lat_);
        bool erase_minX = (coord.lon_ == vertices_[minX_.front()].lon_);
        bool erase_minY = (coord.lat_ == vertices_[minY_.front()].lat_);
        if(!would_be_valid_after_erase(erased_id)){
            valid_defined_ = false;
            wise_defined_ = false;
        }
        vertices_.erase(vertices_.begin()+erased_id);
        area_defined_ = false;
        if(erase_maxX || erase_minX || erase_maxY || erase_minY){
            maxX_.clear();
            minX_.clear();
            maxY_.clear();
            minY_.clear();
            for(uint64_t i = 0;i<vertices_.size();++i){
                update_minX(vertices_[i].lon_,i);
                update_maxX(vertices_[i].lon_,i);
                update_minY(vertices_[i].lat_,i);
                update_maxY(vertices_[i].lat_,i);
            }
        }
        else{
            std::for_each(minX_.begin(),minX_.end(),[erased_id](uint64_t& id){
                if(id>erased_id)
                    --id;
            });
            std::for_each(maxX_.begin(),maxX_.end(),[erased_id](uint64_t& id){
                if(id>erased_id)
                    --id;
            });
            std::for_each(minY_.begin(),minY_.end(),[erased_id](uint64_t& id){
                if(id>erased_id)
                    --id;
            });
            std::for_each(maxY_.begin(),maxY_.end(),[erased_id](uint64_t& id){
                if(id>erased_id)
                    --id;
            });
        }
    }
    size_t number_vertices() const noexcept{
        return vertices_.size();
    }
    const std::vector<Coord>& vertices() const noexcept{
        return vertices_;
    }

    //O(n) difficulty
    bool point_inside(const Coord& point,
            uint32_t max_threads = std::thread::hardware_concurrency()) const noexcept;
    Polygon intersection(const Polygon& other) const noexcept;
    uint64_t define_intersection_by_id(
            const std::vector<std::pair<Coord, Line::id>>& intersections,const std::pair<Coord, Line::id>& ti){
        uint64_t result = std::lower_bound(intersections.begin(),intersections.end(),ti,
            [](const std::pair<Coord, Line::id>& t,const std::pair<Coord, Line::id>& turn)
        {
            return t.second<turn.second;
        }) - intersections.begin();
        return result;
    };
    std::vector<Polygon> cut(
        const Line& line,
        const Polygon& pg,
        bool left_forward
        );
    bool equivalent(const Polygon& other,bool multithread) const noexcept{
        uint64_t number_threads = vertices_.size()/vertices_per_thread_;
        int8_t other_inc = is_clockwise()==other.is_clockwise()?1:-1;
        if(maxX_.size() == other.maxX_.size() && 
            maxY_.size() == other.maxY_.size() &&
            minX_.size() == other.minX_.size() &&
            minY_.size() == other.minY_.size())
            if((!maxX_.empty() && vertices_[maxX_.front()].lon_!=other.vertices_[other.maxX_.front()].lon_) ||
                (!maxY_.empty() && vertices_[maxY_.front()].lat_!=other.vertices_[other.maxY_.front()].lat_) ||
                (!minX_.empty() && vertices_[minX_.front()].lon_!=other.vertices_[other.minX_.front()].lon_) ||
                (!minY_.empty() && vertices_[minY_.front()].lat_!=other.vertices_[other.minY_.front()].lat_))
                return false;
            else{
                std::vector<Coord>::const_iterator iter = 
                    std::find(other.vertices_.begin(),other.vertices_.end(),
                    vertices_.front());
                if(iter==other.vertices_.end())
                    return false;
                else{
                    uint64_t last = vertices_.size()-1;
                    for(uint64_t i = 0, j = std::distance(other.vertices_.begin(),iter);
                        i<=last;++i,j=(j+other_inc+other.vertices_.size())%other.vertices_.size())
                    {
                        uint64_t j_loc = (j+other_inc+other.vertices_.size())%other.vertices_.size();
                        uint64_t i_loc = (i+1+other.vertices_.size())%other.vertices_.size();
                        while(other.vertices_[j]==other.vertices_[j_loc]){
                            j=(j+other_inc+other.vertices_.size())%other.vertices_.size();
                            j_loc = (j+other_inc+other.vertices_.size())%other.vertices_.size();
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
        else return false;

    }
};

void print(const std::vector<std::pair<Coord,Line::id>>& turns);
void print(const Coord& point,std::string_view desc="");
void print(const Polygon& pg);