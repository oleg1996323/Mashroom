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

    auto intersection_openning = [this,left_forward,&other](const Intersection& is){
        const Coord& first_1 = vertices_[is.first_elem_id()];
        const Coord& second_1 = vertices_[is.first_elem_id()+1];
        const Coord& first_2 = other.vertices_[is.second_elem_id()];
        const Coord& second_2 = other.vertices_[is.second_elem_id()+1];
        long double side = ::cross(first_2, second_2, first_1);
        if(side>0)
            return !left_forward;
        else return left_forward; 
    };
    if(!turns.empty())
        in = !intersection_openning(turns.front());
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
