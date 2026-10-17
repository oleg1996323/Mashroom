#include "common/geometry/Polyline.h"

void print(const Coord& point,std::string_view desc="",bool newline = false);

std::vector<Polyline> Polyline::cut_impl(
    const Polyline& other,
    bool left_forward
    ) const noexcept{
    std::vector<Polyline> result;
    std::vector<Intersection> turns = polyline_intersections(other);
    bool in = false;
    //
    if(!turns.empty()){
        const Coord& first_1 = vertices_[turns.front().first_elem_id()];
        const Coord& second_1 = vertices_[turns.front().first_elem_id()+1];
        const Coord& first_2 = vertices_[turns.front().second_elem_id()];
        const Coord& second_2 = vertices_[turns.front().second_elem_id()+1];
        long double side = ::cross(first_2, second_2, first_1);
        if(side>0)
            in=!left_forward;
        else
            in=left_forward;
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
    print(turns);
    result.emplace_back();
    if(in){ //начинается с начальной части полилинии до пересечения
        for(uint64_t i=0;i<=turns.front().first_elem_id();++i){
            print(vertices_[i],"added vertice",true);
            result.back().append(vertices_[i]);
        }
        print(turns.front().point(),"added vertice",true);
        result.back().append(turns.front().point());
        result.emplace_back();
        std::cout<<"emplaced polyline"<<std::endl;
        in = false;
    }
    else {
        print(turns.front().point(),"added vertice",true);
        result.back().append(turns.front().point());
        in = true;
    }
    for(uint64_t i=0,j=1;j<turns.size();++i,++j){
        Polyline* current = &result.back();
        if(turns[j].first_elem_id()==turns[i].first_elem_id()){
            if(in){
                print(turns[j].point(),"added vertice",true);
                current->append(turns[j].point());
                result.emplace_back();
                std::cout<<"emplaced polyline"<<std::endl;
                in = false;
            }
            else{
                const Coord& first_1 = other.vertices_[turns[i].second_elem_id()];
                const Coord& second_1 = other.vertices_[turns[i].second_elem_id()+1];
                const Coord& first_2 = other.vertices_[turns[j].second_elem_id()];
                const Coord& second_2 = other.vertices_[turns[j].second_elem_id()+1];
                if((cross(first_1,second_1,second_2)>0)==
                    (cross(first_2,second_2,second_1)>0)){
                    if(!current->vertices_.empty()){
                        print(current->vertices_.back(),"replaced by");
                        print(turns[j].point());
                        current->vertices_.back() = turns[j].point();
                    }
                    else{
                        print(result.back().vertices_.back(),"replaced by");
                        print(turns[j].point());
                        result.back().vertices_.back() = turns[j].point();
                    }
                }
                else{
                    current->append(turns[j].point());
                    print(turns[j].point(),"added vertice",true);
                    in = true;
                }
            }
        }
        else if(result.size()>1 && 
            !result[result.size()-2].vertices().empty() && 
            turns[j].point()==result[result.size()-2].vertices().back())
        {
            result.pop_back();
            current = &result.back();
            std::cout<<"return to last added polyline"<<std::endl;
            print(*current);
            std::cout<<"continue"<<std::endl;
            in = true;
            continue;
        }
        else{
            if(in){
                for(uint64_t m=j;m<turns.size() && m<turns[j].first_elem_id();++m){
                    print(vertices_[turns[m].first_elem_id()],"added vertice",true);
                    current->append(vertices_[turns[m].first_elem_id()]);
                }
                print(turns[j].point(),"added vertice",true);
                current->append(turns[j].point());
                std::cout<<"emplaced polyline"<<std::endl;
                result.emplace_back();
                in = false;
            }
            else{
                print(turns[j].point(),"added vertice",true);
                current->append(turns[j].point());
                in = true;
            }
        }
    }
    return result;
}

void print(const Coord& point,std::string_view desc,bool newline){
    if(!desc.empty())
        std::cout<<desc.data()<<": ("<<point.lon_<< \
                ", "<<point.lat_<<")";
    else std::cout<<"("<<point.lon_<< \
                ", "<<point.lat_<<")";
    if(newline)
        std::cout<<std::endl;
}

void print(const Polyline& pl){
    std::cout<<'[';
    for(uint64_t first=0,second=1;first<pl.number_segments();++first,++second){
        std::cout<<"(";print(pl.vertices().at(first));std::cout<<",";print(pl.vertices().at(second));std::cout<<")";
        if(second<pl.number_segments())
            std::cout<<",";
    }
    std::cout<<"]"<<std::endl;
}

void print(const std::vector<Polyline>& pls){
    for(const Polyline& pl:pls)
        print(pl);
}

void print(const Intersection& is){
    std::cout<<"intersection: point("<<is.point().lon_<<","<<is.point().lat_<<") "<< \
        "first ID: "<<is.first_elem_id()<<"; "<<"second ID: "<<is.second_elem_id()<<std::endl;
}

void print(const std::vector<Intersection>& iss){
    for(const Intersection& is:iss)
        print(is);
}
