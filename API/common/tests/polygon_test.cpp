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
template<bool CLOCKWISE>
std::vector<turn_info> get_turns(const bg::model::referring_segment<const Coord>& segment,
                                const bg::model::polygon<Coord,CLOCKWISE>& polygon)
{
    std::vector<turn_info> result;
    bg::model::linestring<Coord> seg_ls;
    seg_ls.push_back(segment.first);
    seg_ls.push_back(segment.second);
    bg::detail::self_get_turn_points::no_interrupt_policy policy;
    bg::get_turns<false,false,bg::detail::overlay::assign_null_policy>(
            seg_ls,
            polygon,typename bg::strategies::relate::services::default_strategy<bg::model::linestring<Coord>,
                bg::model::polygon<Coord,CLOCKWISE>>::type(),result,policy);
    return result;
}
template<bool CLOCKWISE>
std::vector<turn_info> get_self_turns(
    const bg::model::polygon<Coord,CLOCKWISE>& polygon)
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

template<bool CLOCKWISE>
int8_t increment(const Coord& first,const Coord& second,bool left_forward){
    bool dy_positive = first.lat_<second.lat_;
    bool dy_zero = first.lat_==second.lat_;
    bool dx_positive = first.lon_<second.lon_;
    bool dx_zero = first.lon_==second.lon_;
    int8_t inc;
    if constexpr (CLOCKWISE==true){
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
    }
    else{
        if(left_forward){
            if(dy_positive)
                inc = -1;
            else{
                if(dy_zero && !dx_positive)
                    inc = -1;
                else
                    inc = 1;
            }
        }
        else{
            if(!dy_positive)
                inc = -1;
            else{
                if(dy_zero && dx_positive)
                    inc = -1;
                else
                    inc = 1;
            }
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

template<typename T>
uint64_t next_id_of_ring(const std::vector<T>& ring,uint64_t id,uint8_t inc){
    return (id+inc+ring.size())%ring.size();
}


//@todo проверить ккасается ли секущая вершины
template<bool CLOCKWISE>
bg::model::multi_polygon<bg::model::polygon<Coord,CLOCKWISE>> cut(const bg::model::linestring<Coord>& polyline,
    const bg::model::polygon<Coord,CLOCKWISE>& pg,
    bool left_forward //оставляем слева и сверху по ходу полилинии (либо квадранты?)
    ){
    bg::model::multi_polygon<bg::model::polygon<Coord,CLOCKWISE>> result;
    for(uint64_t first = 0,second=(first+1)%polyline.size();
         second<polyline.size(); ++first, second=first+1){
        std::vector<turn_info> turns = get_turns(segment_t(polyline[first],polyline[second]),pg);
        //print(turns);
        std::vector<turn_info> by_coord_turns(turns);
        bool in_self_intersection = false;
        std::sort(by_coord_turns.begin(),by_coord_turns.end(),
            [](const turn_info& lhs,const turn_info& rhs)
            {
                if(std::abs(lhs.point.lat_-rhs.point.lat_)< \
                    std::numeric_limits<Lat>::epsilon())
                    return (lhs.point.lon_>rhs.point.lon_);
                else return lhs.point.lat_>rhs.point.lat_;
            });
        std::vector<std::vector<uint64_t>> pgs_turns;
        std::vector<std::vector<uint64_t>> pgs_turns_in_process;
        pgs_turns_in_process.reserve(5);
        int8_t inc = increment<CLOCKWISE>(polyline[first],polyline[second],left_forward);
        int8_t point_id = 0;
        if(left_forward){
            if(CLOCKWISE)
                point_id = 1;
            else point_id = 0;
        }
        else{
            if(CLOCKWISE)
                point_id = 0;
            else point_id = 1;
        }
        for(uint64_t first = 0, second=first+1;second<=turns.size();
            first+=2,second+=2)
        {
            uint64_t turn_id_1 = define_turn_id(turns,by_coord_turns[first]);
            uint64_t turn_id_2 = define_turn_id(turns,by_coord_turns[second]);
            const turn_info& turn_1 = turns[turn_id_1];
            const turn_info& turn_2 = turns[turn_id_2];
            if((turn_id_2+inc+turns.size())%turns.size()==turn_id_1){
                //std::cout<<"new polygon detected"<<std::endl;
                pgs_turns.emplace_back().push_back(turn_id_1);
                pgs_turns.back().push_back(turn_id_2);
                //print(turn_1.point,"inserted openning point");
                //print(turn_2.point,"inserted closing point");
                //std::cout<<"polygon completed"<<std::endl;
            }
            else if(!pgs_turns_in_process.empty() && !pgs_turns_in_process.back().empty()){
                if((turn_id_2+inc+turns.size())%turns.size()==pgs_turns_in_process.back().front())
                {
                    pgs_turns.emplace_back(pgs_turns_in_process.back());
                    pgs_turns.back().push_back(turn_id_1);
                    pgs_turns.back().push_back(turn_id_2);
                    //print(turn_1.point,"inserted openning point");
                    //print(turn_2.point,"inserted closing point");
                    pgs_turns_in_process.pop_back();
                    //std::cout<<"polygon completed"<<std::endl;
                }
                else if( pgs_turns_in_process.back().back()==(turn_id_1+1+turns.size())%turns.size()){
                    pgs_turns_in_process.back().push_back(turn_id_1);
                    pgs_turns_in_process.back().push_back(turn_id_2);
                    //print(turn_1.point,"inserted openning point");
                    //print(turn_2.point,"inserted closing point");
                }
                else{
                    //std::cout<<"new polygon detected"<<std::endl;
                    pgs_turns_in_process.emplace_back().push_back(turn_id_1);
                    pgs_turns_in_process.back().push_back(turn_id_2);
                    //print(turn_1.point,"inserted openning point");
                    //print(turn_2.point,"inserted closing point");
                }
            }
            else{
                //std::cout<<"new polygon detected"<<std::endl;
                pgs_turns_in_process.emplace_back().push_back(turn_id_1);
                pgs_turns_in_process.back().push_back(turn_id_2);
                //print(turn_1.point,"inserted openning point");
                //print(turn_2.point,"inserted closing point");
            }
            //искать пары и записывать для каждого полигона
            //затем линейно пройтись first до second по вершинам сегментов и составить последовательно полигоны
        }
        
        for(const auto& pg_turns:pgs_turns){
            std::cout<<"new polygon"<<std::endl;
            bg::model::polygon<Coord,CLOCKWISE>& new_pg = result.emplace_back();
            for(uint64_t i=0;i<pg_turns.size();++i){
                //открывающий
                if(i%2==0){
                    print(turns[pg_turns[i]].point,"inserted vertice");
                    new_pg.outer().push_back(turns[pg_turns[i]].point);
                }
                else{
                    print(turns[pg_turns[i]].point,"inserted vertice");
                    new_pg.outer().push_back(turns[pg_turns[i]].point);
                    uint64_t this_seg = (turns[pg_turns[i]].operations[1].\
                            seg_id.segment_index+point_id)%pg.outer().size();
                    uint64_t next_i = (i+1+pg_turns.size())%pg_turns.size();
                    uint64_t next_seg = (turns[pg_turns[next_i]].\
                            operations[1].seg_id.segment_index+point_id)%pg.outer().size();
                    for(uint64_t j = this_seg;
                        j!=next_seg;
                        j=(j+inc+pg.outer().size())%pg.outer().size())
                    {
                        print(pg.outer().at(j),"inserted vertice");
                        new_pg.outer().push_back(pg.outer().at(j));
                    }
                    if(new_pg.outer().front()==turns[pg_turns[next_i]].point){
                        print(new_pg.outer().front(),"inserted vertice");
                        new_pg.outer().push_back(new_pg.outer().front());
                    }
                }
            }
            //bg::correct(new_pg);
        }
    }
    return result;
}

TEST(polygon,cut_test_counter){
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
        {
            bg::model::linestring<Coord> polyline = {
                line_t{{.lat_=-1, .lon_=7},
                {.lat_=21, .lon_=7}}
            };
            bg::model::polygon<Coord,false> pg;
            auto& external = pg.outer();
            external.push_back({.lat_=0,.lon_=0});
            external.push_back({.lat_=0,.lon_=10});
            external.push_back({.lat_=3,.lon_=10});
            external.push_back({.lat_=3,.lon_=5});
            external.push_back({.lat_=5.5,.lon_=5});
            external.push_back({.lat_=5.5,.lon_=10});
            external.push_back({.lat_=5,.lon_=10});
            external.push_back({.lat_=5,.lon_=6});
            external.push_back({.lat_=4,.lon_=6});
            external.push_back({.lat_=4,.lon_=11});
            external.push_back({.lat_=7,.lon_=11});
            external.push_back({.lat_=9,.lon_=10});
            external.push_back({.lat_=9,.lon_=6.5});
            external.push_back({.lat_=10,.lon_=6.5});
            external.push_back({.lat_=10,.lon_=8});
            external.push_back({.lat_=10.5,.lon_=8});
            external.push_back({.lat_=10.5,.lon_=6});
            external.push_back({.lat_=8,.lon_=6});
            external.push_back({.lat_=8,.lon_=9});
            external.push_back({.lat_=5.75,.lon_=9});
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
            external.push_back({.lat_=7,.lon_=3});
            external.push_back({.lat_=8,.lon_=3});
            external.push_back({.lat_=12,.lon_=7});
            external.push_back({.lat_=12,.lon_=10});
            external.push_back({.lat_=16,.lon_=10});
            external.push_back({.lat_=16,.lon_=5});
            external.push_back({.lat_=18,.lon_=5});
            external.push_back({.lat_=18,.lon_=10});
            external.push_back({.lat_=20,.lon_=10});
            external.push_back({.lat_=20,.lon_=3});
            external.push_back({.lat_=14,.lon_=3});
            external.push_back({.lat_=14,.lon_=8});
            external.push_back({.lat_=13,.lon_=8});
            external.push_back({.lat_=13,.lon_=0});
            external.push_back({.lat_=0,.lon_=0});
            bg::correct(pg);
            ASSERT_TRUE(bg::is_valid(pg));
            //print(pg);
            std::cout<<std::endl;
            {
                auto res = cut(polyline,pg, false);
                std::vector<bg::model::polygon<Coord,false>> etalon = {
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=9,.lon_=7});
                        result.outer().push_back({.lat_=9,.lon_=10});
                        result.outer().push_back({.lat_=7,.lon_=11});
                        result.outer().push_back({.lat_=4,.lon_=11});
                        result.outer().push_back({.lat_=4,.lon_=7});
                        result.outer().push_back({.lat_=5,.lon_=7});
                        result.outer().push_back({.lat_=5,.lon_=10});
                        result.outer().push_back({.lat_=5.5,.lon_=10});
                        result.outer().push_back({.lat_=5.5,.lon_=7});
                        result.outer().push_back({.lat_=6,.lon_=7});
                        result.outer().push_back({.lat_=6,.lon_=7.5});
                        result.outer().push_back({.lat_=6.5,.lon_=7.5});
                        result.outer().push_back({.lat_=6.5,.lon_=7});
                        result.outer().push_back({.lat_=6.75,.lon_=7});
                        result.outer().push_back({.lat_=6.75,.lon_=8});
                        result.outer().push_back({.lat_=5.75,.lon_=8});
                        result.outer().push_back({.lat_=5.75,.lon_=9});
                        result.outer().push_back({.lat_=8,.lon_=9});
                        result.outer().push_back({.lat_=8,.lon_=7});
                        result.outer().push_back({.lat_=9,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=6.375,.lon_=7});
                        result.outer().push_back({.lat_=6.375,.lon_=7.25});
                        result.outer().push_back({.lat_=6.25,.lon_=7.25});
                        result.outer().push_back({.lat_=6.25,.lon_=7});
                        result.outer().push_back({.lat_=6.375,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=10.5,.lon_=7});
                        result.outer().push_back({.lat_=10.5,.lon_=8});
                        result.outer().push_back({.lat_=10,.lon_=8});
                        result.outer().push_back({.lat_=10,.lon_=7});
                        result.outer().push_back({.lat_=10.5,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=16,.lon_=7});
                        result.outer().push_back({.lat_=16,.lon_=10});
                        result.outer().push_back({.lat_=12,.lon_=10});
                        result.outer().push_back({.lat_=12,.lon_=7});
                        result.outer().push_back({.lat_=13,.lon_=7});
                        result.outer().push_back({.lat_=13,.lon_=8});
                        result.outer().push_back({.lat_=14,.lon_=8});
                        result.outer().push_back({.lat_=14,.lon_=7});
                        result.outer().push_back({.lat_=16,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=20,.lon_=7});
                        result.outer().push_back({.lat_=20,.lon_=10});
                        result.outer().push_back({.lat_=18,.lon_=10});
                        result.outer().push_back({.lat_=18,.lon_=7});
                        result.outer().push_back({.lat_=20,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=3,.lon_=7});
                        result.outer().push_back({.lat_=3,.lon_=10});
                        result.outer().push_back({.lat_=0,.lon_=10});
                        result.outer().push_back({.lat_=0,.lon_=7});
                        result.outer().push_back({.lat_=3,.lon_=7});
                        bg::correct(result);
                        return result;
                    }()
                };
                int counter = 0;
                for(auto& new_pg:res){
                    if(auto found = std::find_if(etalon.begin(),etalon.end(),
                        [&new_pg](const bg::model::polygon<Coord,false>& pg){
                        return bg::equals(pg,new_pg);
                    });found==etalon.end()){
                        std::cout<<"polygon №: "<<counter<<std::endl;
                        print(new_pg);
                        EXPECT_TRUE(false);
                    }
                    else EXPECT_TRUE(true);
                    ++counter;
                }
            }
            {
                auto res = cut(polyline,pg, true);
                std::vector<bg::model::polygon<Coord,false>> etalon = {
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=20,.lon_=7});
                        result.outer().push_back({.lat_=20,.lon_=3});
                        result.outer().push_back({.lat_=14,.lon_=3});
                        result.outer().push_back({.lat_=14,.lon_=7});
                        result.outer().push_back({.lat_=16,.lon_=7});
                        result.outer().push_back({.lat_=16,.lon_=5});
                        result.outer().push_back({.lat_=18,.lon_=5});
                        result.outer().push_back({.lat_=18,.lon_=7});
                        result.outer().push_back({.lat_=20,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=10.5,.lon_=7});
                        result.outer().push_back({.lat_=10.5,.lon_=6});
                        result.outer().push_back({.lat_=8,.lon_=6});
                        result.outer().push_back({.lat_=8,.lon_=7});
                        result.outer().push_back({.lat_=9,.lon_=7});
                        result.outer().push_back({.lat_=9,.lon_=6.5});
                        result.outer().push_back({.lat_=10,.lon_=6.5});
                        result.outer().push_back({.lat_=10,.lon_=7});
                        result.outer().push_back({.lat_=10.5,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=6.75,.lon_=6.75});
                        result.outer().push_back({.lat_=6.5,.lon_=6.75});
                        result.outer().push_back({.lat_=6.5,.lon_=7});
                        result.outer().push_back({.lat_=6.75,.lon_=7});
                        result.outer().push_back({.lat_=6.75,.lon_=6.75});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=5,.lon_=6});
                        result.outer().push_back({.lat_=4,.lon_=6});
                        result.outer().push_back({.lat_=4,.lon_=7});
                        result.outer().push_back({.lat_=5,.lon_=7});
                        result.outer().push_back({.lat_=5,.lon_=6});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,false> result;
                        result.outer().push_back({.lat_=0,.lon_=0});
                        result.outer().push_back({.lat_=0,.lon_=7});
                        result.outer().push_back({.lat_=3,.lon_=7});
                        result.outer().push_back({.lat_=3,.lon_=5});
                        result.outer().push_back({.lat_=5.5,.lon_=5});
                        result.outer().push_back({.lat_=5.5,.lon_=7});
                        result.outer().push_back({.lat_=6,.lon_=7});
                        result.outer().push_back({.lat_=6,.lon_=6.25});
                        result.outer().push_back({.lat_=6.25,.lon_=6.25});
                        result.outer().push_back({.lat_=6.25,.lon_=7});
                        result.outer().push_back({.lat_=6.375,.lon_=7});
                        result.outer().push_back({.lat_=6.375,.lon_=6.5});
                        result.outer().push_back({.lat_=7,.lon_=6.5});
                        result.outer().push_back({.lat_=7,.lon_=3});
                        result.outer().push_back({.lat_=8,.lon_=3});
                        result.outer().push_back({.lat_=12,.lon_=7});
                        result.outer().push_back({.lat_=13,.lon_=7});
                        result.outer().push_back({.lat_=13,.lon_=0});
                        result.outer().push_back({.lat_=0,.lon_=0});
                        bg::correct(result);
                        return result;
                    }()
                };
                int counter = 0;
                for(auto& new_pg:res){
                    if(auto found = std::find_if(etalon.begin(),etalon.end(),
                        [&new_pg](const bg::model::polygon<Coord,false>& pg){
                        return bg::equals(pg,new_pg);
                    });found==etalon.end()){
                        std::cout<<"polygon №: "<<counter<<std::endl;
                        print(new_pg);
                        EXPECT_TRUE(false);
                    }
                    else EXPECT_TRUE(true);
                    ++counter;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////
        /////////////CLOCKWISE////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////
        {
            bg::model::linestring<Coord> polyline = {
                line_t{{.lat_=-1, .lon_=7},
                {.lat_=21, .lon_=7}}
            };
            bg::model::polygon<Coord,true> pg;
            auto& external = pg.outer();
            external.push_back({.lat_=0,.lon_=0});
            external.push_back({.lat_=0,.lon_=10});
            external.push_back({.lat_=3,.lon_=10});
            external.push_back({.lat_=3,.lon_=5});
            external.push_back({.lat_=5.5,.lon_=5});
            external.push_back({.lat_=5.5,.lon_=10});
            external.push_back({.lat_=5,.lon_=10});
            external.push_back({.lat_=5,.lon_=6});
            external.push_back({.lat_=4,.lon_=6});
            external.push_back({.lat_=4,.lon_=11});
            external.push_back({.lat_=7,.lon_=11});
            external.push_back({.lat_=9,.lon_=10});
            external.push_back({.lat_=9,.lon_=6.5});
            external.push_back({.lat_=10,.lon_=6.5});
            external.push_back({.lat_=10,.lon_=8});
            external.push_back({.lat_=10.5,.lon_=8});
            external.push_back({.lat_=10.5,.lon_=6});
            external.push_back({.lat_=8,.lon_=6});
            external.push_back({.lat_=8,.lon_=9});
            external.push_back({.lat_=5.75,.lon_=9});
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
            external.push_back({.lat_=7,.lon_=3});
            external.push_back({.lat_=8,.lon_=3});
            external.push_back({.lat_=12,.lon_=7});
            external.push_back({.lat_=12,.lon_=10});
            external.push_back({.lat_=16,.lon_=10});
            external.push_back({.lat_=16,.lon_=5});
            external.push_back({.lat_=18,.lon_=5});
            external.push_back({.lat_=18,.lon_=10});
            external.push_back({.lat_=20,.lon_=10});
            external.push_back({.lat_=20,.lon_=3});
            external.push_back({.lat_=14,.lon_=3});
            external.push_back({.lat_=14,.lon_=8});
            external.push_back({.lat_=13,.lon_=8});
            external.push_back({.lat_=13,.lon_=0});
            external.push_back({.lat_=0,.lon_=0});
            bg::correct(pg);
            ASSERT_TRUE(bg::is_valid(pg));
            //print(pg);
            {
                auto res = cut(polyline,pg, false);
            
                std::vector<bg::model::polygon<Coord,true>> etalon = {
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=9,.lon_=7});
                        result.outer().push_back({.lat_=9,.lon_=10});
                        result.outer().push_back({.lat_=7,.lon_=11});
                        result.outer().push_back({.lat_=4,.lon_=11});
                        result.outer().push_back({.lat_=4,.lon_=7});
                        result.outer().push_back({.lat_=5,.lon_=7});
                        result.outer().push_back({.lat_=5,.lon_=10});
                        result.outer().push_back({.lat_=5.5,.lon_=10});
                        result.outer().push_back({.lat_=5.5,.lon_=7});
                        result.outer().push_back({.lat_=6,.lon_=7});
                        result.outer().push_back({.lat_=6,.lon_=7.5});
                        result.outer().push_back({.lat_=6.5,.lon_=7.5});
                        result.outer().push_back({.lat_=6.5,.lon_=7});
                        result.outer().push_back({.lat_=6.75,.lon_=7});
                        result.outer().push_back({.lat_=6.75,.lon_=8});
                        result.outer().push_back({.lat_=5.75,.lon_=8});
                        result.outer().push_back({.lat_=5.75,.lon_=9});
                        result.outer().push_back({.lat_=8,.lon_=9});
                        result.outer().push_back({.lat_=8,.lon_=7});
                        result.outer().push_back({.lat_=9,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=6.375,.lon_=7});
                        result.outer().push_back({.lat_=6.375,.lon_=7.25});
                        result.outer().push_back({.lat_=6.25,.lon_=7.25});
                        result.outer().push_back({.lat_=6.25,.lon_=7});
                        result.outer().push_back({.lat_=6.375,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=10.5,.lon_=7});
                        result.outer().push_back({.lat_=10.5,.lon_=8});
                        result.outer().push_back({.lat_=10,.lon_=8});
                        result.outer().push_back({.lat_=10,.lon_=7});
                        result.outer().push_back({.lat_=10.5,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=16,.lon_=7});
                        result.outer().push_back({.lat_=16,.lon_=10});
                        result.outer().push_back({.lat_=12,.lon_=10});
                        result.outer().push_back({.lat_=12,.lon_=7});
                        result.outer().push_back({.lat_=13,.lon_=7});
                        result.outer().push_back({.lat_=13,.lon_=8});
                        result.outer().push_back({.lat_=14,.lon_=8});
                        result.outer().push_back({.lat_=14,.lon_=7});
                        result.outer().push_back({.lat_=16,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=20,.lon_=7});
                        result.outer().push_back({.lat_=20,.lon_=10});
                        result.outer().push_back({.lat_=18,.lon_=10});
                        result.outer().push_back({.lat_=18,.lon_=7});
                        result.outer().push_back({.lat_=20,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=3,.lon_=7});
                        result.outer().push_back({.lat_=3,.lon_=10});
                        result.outer().push_back({.lat_=0,.lon_=10});
                        result.outer().push_back({.lat_=0,.lon_=7});
                        result.outer().push_back({.lat_=3,.lon_=7});
                        bg::correct(result);
                        return result;
                    }()
                };
                int counter = 0;
                for(auto& new_pg:res){
                    if(auto found = std::find_if(etalon.begin(),etalon.end(),
                        [&new_pg](const bg::model::polygon<Coord,true>& pg){
                        return bg::equals(pg,new_pg);
                    });found==etalon.end()){
                        std::cout<<"polygon №: "<<counter<<std::endl;
                        print(new_pg);
                        EXPECT_TRUE(false);
                    }
                    else EXPECT_TRUE(true);
                    ++counter;
                }
            }
            {
                auto res = cut(polyline,pg, true);
                std::vector<bg::model::polygon<Coord,true>> etalon = {
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=20,.lon_=7});
                        result.outer().push_back({.lat_=20,.lon_=3});
                        result.outer().push_back({.lat_=14,.lon_=3});
                        result.outer().push_back({.lat_=14,.lon_=7});
                        result.outer().push_back({.lat_=16,.lon_=7});
                        result.outer().push_back({.lat_=16,.lon_=5});
                        result.outer().push_back({.lat_=18,.lon_=5});
                        result.outer().push_back({.lat_=18,.lon_=7});
                        result.outer().push_back({.lat_=20,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=10.5,.lon_=7});
                        result.outer().push_back({.lat_=10.5,.lon_=6});
                        result.outer().push_back({.lat_=8,.lon_=6});
                        result.outer().push_back({.lat_=8,.lon_=7});
                        result.outer().push_back({.lat_=9,.lon_=7});
                        result.outer().push_back({.lat_=9,.lon_=6.5});
                        result.outer().push_back({.lat_=10,.lon_=6.5});
                        result.outer().push_back({.lat_=10,.lon_=7});
                        result.outer().push_back({.lat_=10.5,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=6.75,.lon_=6.75});
                        result.outer().push_back({.lat_=6.5,.lon_=6.75});
                        result.outer().push_back({.lat_=6.5,.lon_=7});
                        result.outer().push_back({.lat_=6.75,.lon_=7});
                        result.outer().push_back({.lat_=6.75,.lon_=6.75});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=5,.lon_=7});
                        result.outer().push_back({.lat_=5,.lon_=6});
                        result.outer().push_back({.lat_=4,.lon_=6});
                        result.outer().push_back({.lat_=4,.lon_=7});
                        result.outer().push_back({.lat_=5,.lon_=7});
                        bg::correct(result);
                        return result;
                    }(),
                    [](){
                        bg::model::polygon<Coord,true> result;
                        result.outer().push_back({.lat_=0,.lon_=0});
                        result.outer().push_back({.lat_=0,.lon_=7});
                        result.outer().push_back({.lat_=3,.lon_=7});
                        result.outer().push_back({.lat_=3,.lon_=5});
                        result.outer().push_back({.lat_=5.5,.lon_=5});
                        result.outer().push_back({.lat_=5.5,.lon_=7});
                        result.outer().push_back({.lat_=6,.lon_=7});
                        result.outer().push_back({.lat_=6,.lon_=6.25});
                        result.outer().push_back({.lat_=6.25,.lon_=6.25});
                        result.outer().push_back({.lat_=6.25,.lon_=7});
                        result.outer().push_back({.lat_=6.375,.lon_=7});
                        result.outer().push_back({.lat_=6.375,.lon_=6.5});
                        result.outer().push_back({.lat_=7,.lon_=6.5});
                        result.outer().push_back({.lat_=7,.lon_=3});
                        result.outer().push_back({.lat_=8,.lon_=3});
                        result.outer().push_back({.lat_=12,.lon_=7});
                        result.outer().push_back({.lat_=13,.lon_=7});
                        result.outer().push_back({.lat_=13,.lon_=0});
                        result.outer().push_back({.lat_=0,.lon_=0});
                        bg::correct(result);
                        return result;
                    }()
                };
                int counter = 0;
                for(auto& new_pg:res){
                    if(auto found = std::find_if(etalon.begin(),etalon.end(),
                        [&new_pg](const bg::model::polygon<Coord,true>& pg){
                        return bg::equals(pg,new_pg);
                    });found==etalon.end()){
                        std::cout<<"polygon №: "<<counter<<std::endl;
                        print(new_pg);
                        EXPECT_TRUE(false);
                    }
                    else EXPECT_TRUE(true);
                    ++counter;
                }
            }
        }
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
