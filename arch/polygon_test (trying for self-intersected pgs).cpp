#include "common/geometry/Polygon.h"
#include "common/geometry/Point.h"
#include <gtest/gtest.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>

namespace bg = boost::geometry;

TEST(polygon,intersection_test){
    {
        bg::model::polygon<Coord> pg;
        bg::model::linestring<Coord> line = {
            {.lat_=3, .lon_=1},
            {.lat_=-1, .lon_=1}
        };
        auto& external = pg.outer();
        external.push_back({.lat_=0,.lon_=0});
        external.push_back({.lat_=2,.lon_=0});
        external.push_back({.lat_=2,.lon_=2});
        external.push_back({.lat_=0,.lon_=2});
        bg::correct(pg);
        EXPECT_FALSE(bg::intersects(pg));
        auto& boundary=pg.outer();

        // std::cout<<"line"<<std::endl;
        // for(auto& [lon,lat]:boundary)
        //     std::cout<<"("<<lon<<","<<lat<<")";
        // std::cout<<std::endl;

        auto& to_intersect = boundary;

        bg::model::multi_linestring<bg::model::linestring<Coord>> intersection_lines;
        bg::intersection(line,to_intersect, intersection_lines);
        // for(auto& l:intersection_lines)
        //     for(auto& [lat,lon]:l)
        //         std::cout<<"("<<lon<<","<<lat<<")";
        //     std::cout<<std::endl;
        // bg::model::linestring<Coord> intersection_line;
        // bg::intersection(line,to_intersect, intersection_line);
        // for(auto& [lat,lon]:intersection_line)
        //     std::cout<<"("<<lon<<","<<lat<<")";
        // std::cout<<std::endl;
        // bg::model::multi_point<Coord> intersection_points;
        // bg::intersection(line,to_intersect, intersection_points);
        // for(auto& [lat,lon]:intersection_points)
        //     std::cout<<"("<<lon<<","<<lat<<")";
        // std::cout<<std::endl;
    }
    {
        bg::model::polygon<Coord> pg;
        bg::model::linestring<Coord> line = {
            {.lat_=-1, .lon_=1},
            {.lat_=3, .lon_=1},
            {.lat_=3, .lon_=3},
            {.lat_=1, .lon_=3},
            {.lat_=1, .lon_=0}
        };
        auto& external = pg.outer();
        external.push_back({.lat_=0,.lon_=0});
        external.push_back({.lat_=2,.lon_=0});
        external.push_back({.lat_=2,.lon_=2});
        external.push_back({.lat_=0,.lon_=2});
        bg::correct(pg);
        EXPECT_FALSE(bg::intersects(pg));
        auto& boundary=pg.outer();

        // std::cout<<"line"<<std::endl;
        // for(auto& [lon,lat]:boundary)
        //     std::cout<<"("<<lon<<","<<lat<<")";
        // std::cout<<std::endl;

        auto& to_intersect = boundary;

        bg::model::multi_linestring<bg::model::linestring<Coord>> intersection_lines;
        bg::intersection(line,to_intersect, intersection_lines);
        // std::cout<<"intersection by multilinestring: ";
        // for(auto& l:intersection_lines)
        //     for(auto& [lat,lon]:l)
        //         std::cout<<"("<<lon<<","<<lat<<")";
        //     std::cout<<std::endl;
        bg::model::linestring<Coord> intersection_line;
        bg::intersection(line,to_intersect, intersection_line);
        // std::cout<<"intersection by linestring: ";
        // for(auto& [lat,lon]:intersection_line)
        //     std::cout<<"("<<lon<<","<<lat<<")";
        // std::cout<<std::endl;
        bg::model::multi_point<Coord> intersection_points;
        bg::intersection(line,to_intersect, intersection_points);
        // std::cout<<"intersection by points: ";
        // for(auto& [lat,lon]:intersection_points)
        //     std::cout<<"("<<lon<<","<<lat<<")";
        // std::cout<<std::endl;
    }
}

TEST(polygon,self_intersection_test){
    {
        bg::model::polygon<Coord,true> pg;
        auto& external = pg.outer();
        external.push_back({.lat_=0,.lon_=0});
        external.push_back({.lat_=2,.lon_=2});
        external.push_back({.lat_=0,.lon_=2});
        external.push_back({.lat_=2,.lon_=0});
        external.push_back({.lat_=0,.lon_=0});
        EXPECT_TRUE(bg::intersects(pg));
        // std::cout<<"line"<<std::endl;
        // for(auto& [lon,lat]:pg.outer())
        //     std::cout<<"("<<lon<<","<<lat<<")";
        // std::cout<<std::endl;
        // bg::correct(pg);
        // EXPECT_TRUE(bg::intersects(pg));
        // for(auto& [lon,lat]:pg.outer())
        //     std::cout<<"("<<lon<<","<<lat<<")";
        // std::cout<<std::endl;
        typedef bg::detail::overlay::turn_info<Coord> turn_info;
        std::vector<turn_info> turns;
        bg::detail::self_get_turn_points::no_interrupt_policy policy;
        bg::self_turns<bg::detail::overlay::assign_null_policy>(
            pg.outer(), // внешнее кольцо
            bg::strategy::intersection::cartesian_segments<>(),
            turns,
            policy
        );
        // for(auto& turn:turns){
        //     std::cout<<"("<<turn.point.lon_<<","<<turn.point.lat_<<")";
        //     std::cout<<"first segment: "<<turn.operations[0].seg_id.segment_index<<std::endl;
        //     std::cout<<"second segment: "<<turn.operations[1].seg_id.segment_index<<std::endl;
        // }
        EXPECT_EQ(turns.size(),1);

        //
        //  |\   /|
        //  | \ / |
        //  |  X  |
        //  | / \ |
        //  |/   \|
    }
}

bg::model::multi_polygon<bg::model::polygon<Coord,true>> dissolve(const bg::model::polygon<Coord,true>& pg){
    bg::model::multi_polygon<bg::model::polygon<Coord,true>> result;
    typedef bg::detail::overlay::turn_info<Coord> turn_info;
    std::vector<turn_info> turns;
    bg::detail::self_get_turn_points::no_interrupt_policy policy;
    bg::self_turns<bg::detail::overlay::assign_null_policy>(
        pg.outer(), // внешнее кольцо
        bg::strategy::intersection::cartesian_segments<>(),
        turns,
        policy
    );
    std::vector<const turn_info*> in_turns = [&vertices = pg.outer()](const std::vector<turn_info>& turns){
        std::vector<const turn_info*> result(turns.size());
        for(uint64_t i=0;i<turns.size();++i)
            result[i]=&turns[i];
        std::sort(result.begin(),result.end(),[](const turn_info* lhs,const turn_info* rhs){
            if(lhs->operations[0].seg_id.segment_index<rhs->operations[0].seg_id.segment_index)
                return lhs->operations[1].seg_id.segment_index<rhs->operations[1].seg_id.segment_index;
            else return false;
        });
        return result;
    }(turns);
    std::vector<const turn_info*> out_turns = [](const std::vector<turn_info>& turns){
        std::vector<const turn_info*> result(turns.size());
        for(uint64_t i=0;i<turns.size();++i)
            result[i]=&turns[i];
        std::sort(result.begin(),result.end(),[](const turn_info* lhs,const turn_info* rhs){
            if(lhs->operations[1].seg_id.segment_index<rhs->operations[1].seg_id.segment_index)
                return lhs->operations[0].seg_id.segment_index<rhs->operations[0].seg_id.segment_index;
            else return false;
        });
        return result;
    }(turns);
    //making only one polygon by starting from in-turn vertice
    for(const turn_info* in:in_turns){
        bg::model::polygon<Coord> new_pg;
        new_pg.outer().push_back(in->point);
        bool is_in = true;
        uint64_t current_segment = in->operations[1].seg_id.segment_index;
        for(;;current_segment=((current_segment+(is_in?1+pg.outer().size():-1+pg.outer().size()))%pg.outer().size())){
            std::vector<const turn_info*>::const_iterator lower;
            std::vector<const turn_info*>::const_iterator upper;
            if(is_in){
                lower = std::lower_bound(out_turns.begin(),out_turns.end(),current_segment,[](const turn_info* cont_val,uint64_t val){
                    return cont_val->operations[1].seg_id.segment_index<val;
                });
                upper = std::upper_bound(out_turns.begin(),out_turns.end(),current_segment,[](uint64_t val,const turn_info* cont_val){
                    return val<cont_val->operations[1].seg_id.segment_index;
                });
            }
            else{
                lower = std::lower_bound(in_turns.begin(),in_turns.end(),current_segment,[](const turn_info* cont_val,uint64_t val){
                    return val>cont_val->operations[0].seg_id.segment_index;
                });
                upper = std::upper_bound(in_turns.begin(),in_turns.end(),current_segment,[](uint64_t val,const turn_info* cont_val){
                    return val<cont_val->operations[0].seg_id.segment_index;
                });
            }
            if(lower==upper){
                new_pg.outer().push_back(pg.outer().at(is_in?((current_segment-1+pg.outer().size())%pg.outer().size()):current_segment));
                continue;
            }
            else{
                std::vector<const turn_info*>::const_iterator found;
                if(is_in)
                    found = std::find_if(lower,upper,[current_segment](const turn_info* turn){
                        return turn->operations[0].seg_id.segment_index==current_segment;
                    });
                else found = std::find_if(lower,upper,[seg = current_segment-1](const turn_info* turn){
                        return turn->operations[1].seg_id.segment_index==seg;
                    });
                if(found == upper){
                    new_pg.outer().push_back(pg.outer().at(is_in?((current_segment-1+pg.outer().size())%pg.outer().size()):current_segment));
                    is_in=!is_in;
                    continue;
                }
                else{
                    new_pg.outer().push_back((*lower)->point);
                    is_in=!is_in;
                    if((*lower)->point==in->point){
                        result.push_back(new_pg);
                        break;
                    }
                    else continue;
                }
            }
        }
    }
    return result;
}

typedef bg::detail::overlay::turn_info<Coord> turn_info;
typedef bg::model::referring_segment<const Coord> segment_t;
std::vector<turn_info> get_turns(const bg::model::referring_segment<const Coord>& segment,
                                const bg::model::polygon<Coord,true>& polygon)
{
    std::vector<turn_info> result;
    bg::model::linestring<Coord> seg_ls;
    seg_ls.push_back(segment.first);
    seg_ls.push_back(segment.second);
    bg::detail::self_get_turn_points::no_interrupt_policy policy;
    bg::get_turns<false,false,bg::detail::overlay::assign_null_policy>(
            seg_ls,
            polygon,bg::strategies::relate::services::default_strategy<bg::model::linestring<Coord>,
                bg::model::polygon<Coord,true>>::type(),result,policy);
    return result;
}

std::vector<turn_info> get_self_turns(
    const bg::model::polygon<Coord,true>& polygon)
{
    std::vector<turn_info> result;
    bg::detail::self_get_turn_points::no_interrupt_policy policy;
    //boost::geometry::model::ring<Coord, true, true, std::vector, std::allocator>
    //inline const std::vector<...> &boost::geometry::model::polygon<...>::inners() const
    bg::self_turns<bg::detail::overlay::assign_null_policy>(
        polygon.outer(), // внешнее кольцо
        bg::strategy::intersection::cartesian_segments<>(),
        result,
        policy
    );
    return result;
}

enum class SIDE{
    ON = 1,
    LEFT = (1<<1),
    RIGHT = (1<<2),
    LEFT_ON = LEFT|ON,
    RIGHT_ON = RIGHT|ON
};

SIDE point_side(const Coord& first,const Coord& second, const Coord& point){
    int side_int = bg::strategy::side::side_by_triangle<>::apply(first, second, point);
    SIDE result;
    if(side_int<0)
        result=static_cast<SIDE>(static_cast<size_t>(result)|static_cast<size_t>(SIDE::LEFT));
    if(side_int==0)
        result=static_cast<SIDE>(static_cast<size_t>(result)|static_cast<size_t>(SIDE::ON));
    if(side_int>0)
        result=static_cast<SIDE>(static_cast<size_t>(result)|static_cast<size_t>(SIDE::RIGHT));
    return result;
}

int8_t increment(const Coord& first,const Coord& second,bool left_forward){
    bool dy_positive = first.lat_<second.lat_;
    bool dy_zero = first.lat_==second.lat_;
    bool dx_positive = first.lon_<second.lon_;
    bool dx_zero = first.lon_==second.lon_;
    int8_t inc;
    if(left_forward){
        if(dy_positive)
            inc = 1;
        else{
            if(dy_zero && !dx_positive)
                inc = 1;
            else
                inc = -1;
        }
    }
    else{
        if(!dy_positive)
            inc = 1;
        else{
            if(dy_zero && dx_positive)
                inc = 1;
            else
                inc = -1;
        }
    }
    return inc;
}



uint64_t define_turn_id_by_coord(const std::vector<turn_info>& turns_by_coord,const Coord& from){
    uint64_t next_by_coord = std::lower_bound(turns_by_coord.begin(),turns_by_coord.end(),from,
        [](const turn_info& t,const Coord& point)
    {
        if(point.lat_==t.point.lat_)
            return t.point.lon_>point.lon_;
        return t.point.lat_>point.lat_;
    })-turns_by_coord.begin();
    return next_by_coord;
}

uint64_t define_turn_id(const std::vector<turn_info>& turns,const turn_info& ti){
    uint64_t result = std::lower_bound(turns.begin(),turns.end(),ti,
        [](const turn_info& t,const turn_info& turn)
    {
        return t.operations[1].seg_id.segment_index<turn.operations[1].seg_id.segment_index;
    }) - turns.begin();
    return result;
}

std::vector<turn_info>::const_iterator find_turn_by_segment_id(const std::vector<turn_info>& turns,uint64_t segment_id){
    std::vector<turn_info>::const_iterator result;
    result = std::lower_bound(turns.begin(),turns.end(),segment_id,
        [](const turn_info& t,uint64_t id)
    {
        return t.operations[1].seg_id.segment_index<id;
    });
    return result;
}

void observe_to_next_intersection(
    uint64_t from,
    const bg::model::polygon<Coord,true>& polygon,
    const segment_t& intersecting_segment,
    const std::vector<turn_info>& turns,
    const std::vector<turn_info>& self_turns,
    bool from_in_self_intersection,
    bool& to_in_self_intersection,
    bool left_forward){
    for(uint64_t seg = (turns[from].operations[1].seg_id.segment_index+1)%polygon.outer().size();
            seg<=turns[(from+1)%turns.size()].operations[1].seg_id.segment_index;
            seg=(seg+1)%polygon.outer().size())
    {
        auto found = find_turn_by_segment_id(turns,seg);
        auto found_self = find_turn_by_segment_id(self_turns,seg);
        if(found_self!=self_turns.end()){
            if(found!=turns.end()){
                SIDE side = point_side(intersecting_segment.first,intersecting_segment.second,found_self->point);
                if(left_forward){
                    switch(side){
                        case SIDE::LEFT:
                        case SIDE::LEFT_ON:
                        case SIDE::ON:
                            to_in_self_intersection=!to_in_self_intersection;
                    }
                }
                else{
                    switch(side){
                        case SIDE::RIGHT:
                        case SIDE::RIGHT_ON:
                        case SIDE::ON:
                            to_in_self_intersection=!to_in_self_intersection;
                    }
                }
                return;
            }
            else to_in_self_intersection = !to_in_self_intersection;
        }
    }
}

size_t number_vertices_between_intersections(const std::vector<turn_info>& turns,uint64_t from,uint64_t to){
    size_t result = 0;
    for(uint64_t second = to,
                first = (second-1+turns.size())%turns.size();
                second>from;
                second=(second-1+turns.size())%turns.size(),
                first=(first-1+turns.size())%turns.size())
    {
        result+=
            turns[second].operations[1].seg_id.segment_index-
            turns[first].operations[1].seg_id.segment_index;
    }
    return result;
}

void insert_to_next_intersection (
        const bg::model::polygon<Coord>& initial_pg,
        bg::model::polygon<Coord>& modified_pg,
        const std::vector<turn_info>& turns,
        uint64_t from_id,
        uint64_t to_id,
        int8_t point_id,
        bool in_self_isection_from,
        bool in_self_isection_to)
{
    uint64_t segments_sz = turns[(to_id)%turns.size()].operations[1].seg_id.segment_index - 
            turns[from_id].operations[1].seg_id.segment_index;
    if(in_self_isection_from!=in_self_isection_to){
        size_t before_resize = modified_pg.outer().size();
        uint64_t number_vertices = number_vertices_between_intersections(turns,from_id,to_id);
        turns[(from_id+1)%turns.size()].operations[1].seg_id.segment_index - 
                turns[from_id].operations[1].seg_id.segment_index;
        modified_pg.outer().resize(modified_pg.outer().size()+segments_sz+to_id-from_id+1);
        modified_pg.outer().at(modified_pg.outer().size()-1-segments_sz)=turns[from_id].point;
        modified_pg.outer().back() = turns[(from_id+1)%turns.size()].point;
        
        //последовательная вставка пересечения to_id, вершин между пересечениями и пересечения from_id
        for(uint64_t second = to_id,
                    first = (second-1+turns.size())%turns.size();
                    second>from_id;
                    second=(second-1+turns.size())%turns.size(),
                    first=(first-1+turns.size())%turns.size())
        {
            uint64_t start_seg = turns[second].operations[1].seg_id.segment_index;
            modified_pg.outer().at(start_seg-1)=turns[first].point;
            for(size_t i = start_seg;i>before_resize;--i)
                modified_pg.outer().at(i)=initial_pg.outer().at(start_seg+point_id);
        }
    }
    else {
        // next_id = (from_id+1)%turns.size();
        // modified_pg.outer().push_back(turns[next_id].point);
    }
};

void print(const std::vector<turn_info>& turns){
    uint64_t i = 0;       
    for(auto& turn:turns){
        std::cout<<i<<" point: ("<<turn.point.lon_<<","<<turn.point.lat_<<")"<<std::endl;
        std::cout<<"polyline segment: "<<turn.operations[0].seg_id.segment_index<<std::endl;
        std::cout<<"polygon segment: "<<turn.operations[1].seg_id.segment_index<<std::endl;
        ++i;
    }
    std::cout<<std::endl;
}

void print(const Coord& point,std::string_view desc=""){
    if(!desc.empty())
        std::cout<<desc.data()<<": ("<<point.lon_<< \
                ", "<<point.lat_<<")"<<std::endl;
    else std::cout<<"("<<point.lon_<< \
                ", "<<point.lat_<<")"<<std::endl;
}

template<bool CLOCKWISE>
void print(const bg::model::polygon<Coord,CLOCKWISE>& pg){
    std::cout<<"outer: ";
    if(!pg.outer().empty()){
        for(auto& vertice:pg.outer())
            print(vertice);
    }
    else std::cout<<"NaN";
    std::cout<<std::endl;
    std::cout<<"inners: ";
    if(!pg.inners().empty()){
        uint64_t i = 0;
        for(auto& inner:pg.inners()){
            std::cout<<i<<". ";
            if(!inner.empty()){
                for(auto& vertice:inner)
                    print(vertice);
            }
            else std::cout<<"NaN"<<std::endl;
        }
    }
    else std::cout<<"NaN"<<std::endl;
}

bg::model::multi_polygon<bg::model::polygon<Coord>> cut(const bg::model::linestring<Coord>& polyline,
    const bg::model::polygon<Coord,true>& pg,
    bool left_forward //оставляем слева и сверху по ходу полилинии (либо квадранты?)
    ){
    bg::model::multi_polygon<bg::model::polygon<Coord>> result;
    for(uint64_t first = 0,second=(first+1)%polyline.size();
        second<polyline.size(); ++first, second=first+1)
    {
        std::vector<bg::model::polygon<Coord>> pgs_in_process;
        bg::model::polygon<Coord,true>& new_pg = pgs_in_process.emplace_back();
        segment_t intersecting_segment(polyline[first],polyline[second]);
        std::vector<turn_info> turns = get_turns(intersecting_segment,pg);
        std::vector<turn_info> self_turns = get_self_turns(pg);
        self_turns.reserve(self_turns.size()*2);
        for(auto& t:self_turns){
            turn_info tmp_turn = t;
            std::swap(tmp_turn.operations[0],tmp_turn.operations[1]);
            self_turns.push_back(std::move(tmp_turn));
        }
        std::sort(self_turns.begin(),self_turns.end(),[](const turn_info& lhs,const turn_info& rhs){
            return lhs.operations[0].seg_id.segment_index<rhs.operations[1].seg_id.segment_index;
        });
        
        print(turns);
        print(self_turns);
        uint64_t turns_offset = std::max_element(turns.begin(),turns.end(),
            [](const turn_info& lhs,const turn_info& rhs)
        {
            return lhs.point.lat_<
                rhs.point.lat_;
        })-turns.begin();
        uint64_t turn_id = turns_offset;
        //на случай, если несколько точек пересечения в одной координате
        uint64_t current_oppening_id= turn_id;
        int8_t inc = increment(polyline[first],polyline[second],left_forward);
        int8_t point_id = left_forward?0:1;
        bool in_self_isection_open = false;
        bool in_self_isection_close = false;
        while(true){
            auto& openning_point = turns[current_oppening_id].point;
            print(openning_point,"openning point");
            new_pg.outer().push_back(openning_point);
            uint64_t next_point_id = (current_oppening_id+1)%turns.size();
            Coord next_point = turns[next_point_id].point;
            //поиск замыкающего пересечения
            //если in_self_intersection, то искать следующее пересечение, которое !in_self_intersection; делаем resize и помещаем все вершины между пересечениями
            observe_to_next_intersection(current_oppening_id,pg,intersecting_segment,turns,self_turns,in_self_isection_open,in_self_isection_close,left_forward);
            if(in_self_isection_close!=in_self_isection_open){
                if(next_point.lat_>=openning_point.lat_ && next_point.lon_>=openning_point.lon_){
                    new_pg.outer().push_back(next_point);
                }
                else{
                    new_pg = pgs_in_process.emplace_back();
                    std::cout<<"new polygon detected"<<std::endl;
                    current_oppening_id = next_point_id;
                    continue;
                }
            }
            else{
                if(next_point.lat_<=openning_point.lat_ && next_point.lon_<=openning_point.lon_){
                    new_pg.outer().push_back(next_point);
                }
                else{
                    new_pg = pgs_in_process.emplace_back();
                    std::cout<<"new polygon detected"<<std::endl;
                    current_oppening_id = next_point_id;
                    continue;
                }
            }
            print(next_point,"closing point");
            next_point_id = (next_point_id+1)%turns.size();
            next_point = turns[next_point_id].point;
            //поиск нового открывающего пересечения
            if(in_self_isection_close!=in_self_isection_open){
                if(next_point.lat_>=openning_point.lat_ && next_point.lon_>=openning_point.lon_)
                    break;
                else{
                    new_pg = pgs_in_process.emplace_back();
                    std::cout<<"new polygon detected"<<std::endl;
                    current_oppening_id = next_point_id;
                    continue;
                }
            }
            else{
                if(next_point.lat_<=openning_point.lat_ && next_point.lon_<=openning_point.lon_)
                    new_pg.outer().push_back(next_point);
                else{
                    new_pg = pgs_in_process.emplace_back();
                    std::cout<<"new polygon detected"<<std::endl;
                    current_oppening_id = next_point_id;
                    continue;
                }
            }
            current_oppening_id = next_point_id;
            print(next_point,"openning point");
            //insert_to_next_intersection(pg,new_pg,turns,current_oppening_id,point_id,in_self_isection_open,in_self_isection_close);
            //print(new_pg);
        }
    }
    return result;
}

TEST(polygon,cut_test){
    {   
        //      ________
        //     |        |
        //  |\ | /|     |
        // 1|_\|/_|_____|
        //  |  X  |     
        //  | /|\ |
        // 1|/ | \|
        //  0  |
        //     0
        using line_t = bg::model::linestring<Coord>;
        bg::model::multi_linestring<bg::model::linestring<Coord>> polyline = {
            line_t{{.lat_=-1, .lon_=1},
            {.lat_=3, .lon_=1}},
            line_t{{.lat_=3, .lon_=1},
            {.lat_=3, .lon_=3}},
            line_t{{.lat_=3, .lon_=3},
            {.lat_=1, .lon_=3}},
            line_t{{.lat_=1, .lon_=3},
            {.lat_=1, .lon_=0}}
        };
        bg::model::polygon<Coord,true> pg;
        auto& external = pg.outer();
        external.push_back({.lat_=0,.lon_=0});
        external.push_back({.lat_=2,.lon_=2});
        external.push_back({.lat_=0,.lon_=2});
        external.push_back({.lat_=2,.lon_=0});
        external.push_back({.lat_=0,.lon_=0});
        bg::correct(pg);
    }
    {   
        // 14______|_13
        //  |      | /
        //  |      |/
        //  |      /
        //  |     /|
        //  |    / |
        //  | 12/  |
        //  |  | 7_|____6
        //  |  | |_|_9  |
        //  |  |_8_|_|  |
        //  | 11 __|_10_|5
        //  |  4|  | _ 
        //  |   |  |/ |2
        //  |   |  |  |
        // 0|___|_/|__|1
        //      |/ |
        //      3
        //
        using line_t = bg::model::linestring<Coord>;
        bg::model::linestring<Coord> polyline = {
            line_t{{.lat_=-1, .lon_=7},
            {.lat_=21, .lon_=7}}
        };
        bg::model::polygon<Coord,true> pg;
        auto& external = pg.outer();
        external.push_back({.lat_=0,.lon_=0});
        external.push_back({.lat_=0,.lon_=10});
        external.push_back({.lat_=3,.lon_=10});
        external.push_back({.lat_=-1,.lon_=5});
        external.push_back({.lat_=5.5,.lon_=5});
        external.push_back({.lat_=5.5,.lon_=10});
        external.push_back({.lat_=5,.lon_=10});
        external.push_back({.lat_=5,.lon_=6});
        external.push_back({.lat_=4,.lon_=6});
        external.push_back({.lat_=4,.lon_=11});
        external.push_back({.lat_=7,.lon_=11});
        external.push_back({.lat_=9,.lon_=10});
        external.push_back({.lat_=8,.lon_=6});
        external.push_back({.lat_=9,.lon_=6});
        external.push_back({.lat_=8,.lon_=9});
        external.push_back({.lat_=7,.lon_=3});
        external.push_back({.lat_=5.75,.lon_=6.5});
        external.push_back({.lat_=5.75,.lon_=8});
        external.push_back({.lat_=6.75,.lon_=8});
        external.push_back({.lat_=6.75,.lon_=6.75});
        external.push_back({.lat_=6.5,.lon_=6.75});
        external.push_back({.lat_=6.5,.lon_=7.5});
        external.push_back({.lat_=6,.lon_=7.5});
        external.push_back({.lat_=6,.lon_=6.25});
        external.push_back({.lat_=6.25,.lon_=6.25});
        external.push_back({.lat_=6.25,.lon_=7.25});
        external.push_back({.lat_=6.375,.lon_=7.25});
        external.push_back({.lat_=6.375,.lon_=6.5});
        external.push_back({.lat_=7,.lon_=6.5});
        external.push_back({.lat_=7,.lon_=9});
        external.push_back({.lat_=8,.lon_=3});
        external.push_back({.lat_=12,.lon_=7});
        external.push_back({.lat_=12,.lon_=10});
        external.push_back({.lat_=16,.lon_=10});
        external.push_back({.lat_=16,.lon_=5});
        external.push_back({.lat_=18,.lon_=5});
        external.push_back({.lat_=20,.lon_=10});
        external.push_back({.lat_=18,.lon_=10});
        external.push_back({.lat_=20,.lon_=3});
        external.push_back({.lat_=14,.lon_=3});
        external.push_back({.lat_=14,.lon_=8});
        external.push_back({.lat_=13,.lon_=8});
        external.push_back({.lat_=13,.lon_=0});
        external.push_back({.lat_=0,.lon_=0});

        bg::correct(pg);
        //assert(bg::is_valid(pg)); @todo dissolve self-intersections
        typedef bg::detail::overlay::turn_info<Coord> turn_info;
        std::vector<turn_info> turns;
        bg::detail::self_get_turn_points::no_interrupt_policy policy;
        bg::get_turns<false,false,bg::detail::overlay::assign_null_policy>(
            //bg::model::polygon<Coord,true>,bg::model::linestring<Coord>,2,2
            polyline,pg,bg::strategies::relate::services::default_strategy<bg::model::linestring<Coord>,bg::model::polygon<Coord,true>>::type(),turns,policy);
        // for(auto& turn:turns){
        //     std::cout<<"point: ("<<turn.point.lon_<<","<<turn.point.lat_<<")"<<std::endl;
        //     std::cout<<"polyline segment: "<<turn.operations[0].seg_id.segment_index<<std::endl;
        //     std::cout<<"polygon segment: "<<turn.operations[1].seg_id.segment_index<<std::endl;
        // }
        std::cout<<std::endl;
        auto res = cut(polyline,pg, true);
        for(auto it_pg = res.begin();it_pg!=res.end();++it_pg){
            std::cout<<"Polygon ["<<it_pg-res.begin()<<"]\n";
            bg::correct(*it_pg);
            for(auto& vertice:it_pg->outer()){
                std::cout<<"("<<vertice.lon_<<", "<<vertice.lat_<<");";
            }
            std::cout<<std::endl;
        }
        // typedef bg::detail::overlay::turn_info<Coord> turn_info;
        // std::vector<turn_info> turns;
        // bg::detail::self_get_turn_points::no_interrupt_policy policy;
        // bg::self_turns<bg::detail::overlay::assign_null_policy>(
        //     pg.outer(), // внешнее кольцо
        //     bg::strategy::intersection::cartesian_segments<>(),
        //     turns,
        //     policy
        // );
        // bool is_in = false;
        // for(auto& turn:turns){
        //     auto first = turn.operations[0].seg_id.segment_index;
        //     auto second = turn.operations[1].seg_id.segment_index;
        //     if(pg.outer().at(first)<pg.outer().at(second))
        //     std::cout<<"first segment"<<turn.operations[0].seg_id.segment_index<<std::endl;
        //     std::cout<<"second segment"<<turn.operations[1].seg_id.segment_index<<std::endl;
        // }
        // std::cout<<std::endl;
    }
}

TEST(polygon,dissolve_test){
    // bg::model::polygon<Coord,true> pg;
    // auto& external = pg.outer();
    // external.push_back({.lat_=0,.lon_=0});
    // external.push_back({.lat_=3,.lon_=3});
    // external.push_back({.lat_=3,.lon_=0});
    // external.push_back({.lat_=0,.lon_=3});
    // external.push_back({.lat_=6,.lon_=1.5});
    // external.push_back({.lat_=0,.lon_=0});
    // auto pgs = dissolve(pg);
    // for(auto& dpg:pgs){
    //     for(auto& [lat,lon]:dpg.outer())
    //         std::cout<<"("<<lon<<","<<lat<<")";
    //     std::cout<<std::endl;
    // }

}

int main(int argc,char* argv[]){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
