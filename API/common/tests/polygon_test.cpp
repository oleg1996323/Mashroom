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

void cut(const bg::model::multi_linestring<bg::model::linestring<Coord>>& polyline,
    const bg::model::polygon<Coord,true>& pg){
    for(auto& line:polyline){
        bg::model::polygon<Coord,true> new_pg;
        typedef bg::detail::overlay::turn_info<Coord> turn_info;
        std::vector<turn_info> turns;
        bg::detail::self_get_turn_points::no_interrupt_policy policy;
        bg::get_turns<false,false,bg::detail::overlay::assign_null_policy>(
            //bg::model::polygon<Coord,true>,bg::model::linestring<Coord>,2,2
            line,pg,bg::strategies::relate::services::default_strategy<bg::model::linestring<Coord>,bg::model::polygon<Coord,true>>::type(),turns,policy);
        std::vector<turn_info> by_coord_turns(turns);
        std::sort(by_coord_turns.begin(),by_coord_turns.end(),
            [](const turn_info& lhs,const turn_info& rhs)
            {
                return lhs.point.lat_<rhs.point.lat_; 
            });
        
        for(auto& turn:turns){
            std::cout<<"point: ("<<turn.point.lon_<<","<<turn.point.lat_<<")"<<std::endl;
            std::cout<<"polyline segment: "<<turn.operations[0].seg_id.segment_index<<std::endl;
            std::cout<<"polygon segment: "<<turn.operations[1].seg_id.segment_index<<std::endl;
        }
        std::cout<<std::endl;
        uint64_t turns_offset = std::min_element(turns.begin(),turns.end(),
            [](const turn_info& lhs,const turn_info& rhs)
        {
            return lhs.operations[1].seg_id.segment_index<
                rhs.operations[1].seg_id.segment_index;
        })-turns.begin();
        uint64_t turn_id = turns_offset;
        while(true){
            auto& openning_point = turns[turn_id].point;
            std::cout<<"openning point: ("<<openning_point.lon_<< \
                ", "<<openning_point.lat_<<");"<<std::endl;
            new_pg.outer().push_back(openning_point);
            uint64_t closing_id = std::lower_bound(by_coord_turns.begin(),by_coord_turns.end(),openning_point,
                [](const turn_info& t,const Coord& point)
            {
                if(point.lat_==t.point.lat_)
                    return t.point.lon_<point.lon_;
                return t.point.lat_<point.lat_;
            }) - by_coord_turns.begin()+1;
            if(closing_id==by_coord_turns.size())
                break;
            auto& closing_point = turns[closing_id].point;
            std::cout<<"openning id="<<openning_id<<"; closing id="<<closing_id<<std::endl;
            std::cout<<"Search openning from ("<<closing_point.lon_<<", "<<\
                closing_point.lat_<<")"<<std::endl;
            openning_id = find_next(turns,closing_id);
            //если полигон замыкается на начальной точке пересечения
            if(intersection_info.back().turns.front()==openning_id){
                //добавляем 
                if(!passed.empty())
                    intersection_info.emplace_back().turns.push_back(passed.back().first);
                else break;
            }
            //если точки пересечения не смежные, то вставляем пропущенные
            if(((openning_id-closing_id+turns.size())%turns.size())>1){
                passed.push_back({
                    (closing_id+1)%turns.size(),
                    (openning_id-1+turns.size())%turns.size()});
            }
            else{
                intersection_info.back().turns.push_back(openning_id);
                closing_id = (openning_id+1+turns.size())%turns.size();
            }
            std::cout<<"Current state:"<<std::endl;
            for(auto& i:intersection_info.back().turns){
                auto& turn = turns[i].point;
                std::cout<<"("<<turn.lon_<<", "<<turn.lat_<<");";
            }
            std::cout<<std::endl;
        }




        bool in = false;
        bool reverse_side = false;
        if(turns.front().point.lon_==turns.back().point.lon_ &&
            turns.front().point.lat_==turns.back().point.lat_)
            return;
        
        const bool dx_positive = turns[turns_offset].point.lon_<
            turns[(turns_offset-1+turns.size())%turns.size()].point.lon_;
        const bool dy_positive = turns[turns_offset].point.lat_<
            turns[(turns_offset-1+turns.size())%turns.size()].point.lat_;
        std::function<bool(bool is_in, const Coord &first, const Coord &second)> codirected;
        //проверяет сонаправленность точек песечения в соответствии с положительностью/отрицательностью dx и dy
        codirected = [&codirected,dx_positive,dy_positive](bool is_in,const Coord& first,const Coord& second) ->bool
        {
            if(!is_in)
                if(dy_positive){
                    if(first.lat_<=second.lat_){
                        if(dx_positive)
                            return first.lon_<=second.lon_;
                        else return first.lon_>=second.lon_;
                    }
                    else return false;
                }
                else{
                    if(first.lat_>=second.lat_){
                        if(dx_positive)
                            return first.lon_<=second.lon_;
                        else return first.lon_>=second.lon_;
                    }
                    else return false;
                }
            else return codirected(!is_in,first,second);
        };
        //находит следующее пересечение, с которым будет соединение
        auto find_next = [&codirected,&turns_offset]
            (const std::vector<turn_info>& t,uint64_t current) mutable
        {
            for(uint64_t next = (current+1)%t.size();
                ;
                next=(next+1)%t.size())
            {
                if((current-turns_offset+t.size())%t.size()<
                    (next-turns_offset+t.size())%t.size()){
                    if(codirected(true,t[current].point,t[next].point))
                        return next;
                }
                else if((current-turns_offset+t.size())%t.size()>
                    (next-turns_offset+t.size())%t.size()){
                    if(codirected(true,t[next].point,t[current].point))
                        return next;
                }
                else assert(false);
            }
            return uint64_t(0);
        };
        struct SidedIntersectionInfo{
            std::vector<uint64_t> turns;
            bool left = false;
            bool linked = false;
        };
        bool left_side = true;
        std::vector<SidedIntersectionInfo> intersection_info;
        std::vector<std::pair<uint64_t,uint64_t>> passed;
        uint64_t turn_id = turns_offset;
        // //индекс сегмента совпадает с индексом вершины
        // auto first_intersection = turns[turn_id].operations[1].seg_id.segment_index;
        // //индекс сегмента на 1 больше индекса вершины
        // auto second_intersection = turns[(turn_id+1)%turns.size()].operations[1].seg_id.segment_index;
        intersection_info.emplace_back().turns.push_back(turn_id);
        uint64_t openning_id = turn_id;
        uint64_t closing_id = (openning_id+1+turns.size())%turns.size();
        
        while(true){
            intersection_info.back().turns.push_back(closing_id);
            auto& closing_point = turns[closing_id].point;
            std::cout<<"openning id="<<openning_id<<"; closing id="<<closing_id<<std::endl;
            std::cout<<"Search openning from ("<<closing_point.lon_<<", "<<\
                closing_point.lat_<<")"<<std::endl;
            openning_id = find_next(turns,closing_id);
            //если полигон замыкается на начальной точке пересечения
            if(intersection_info.back().turns.front()==openning_id){
                //добавляем 
                if(!passed.empty())
                    intersection_info.emplace_back().turns.push_back(passed.back().first);
                else break;
            }
            //если точки пересечения не смежные, то вставляем пропущенные
            if(((openning_id-closing_id+turns.size())%turns.size())>1){
                passed.push_back({
                    (closing_id+1)%turns.size(),
                    (openning_id-1+turns.size())%turns.size()});
            }
            else{
                intersection_info.back().turns.push_back(openning_id);
                closing_id = (openning_id+1+turns.size())%turns.size();
            }
            std::cout<<"Current state:"<<std::endl;
            for(auto& i:intersection_info.back().turns){
                auto& turn = turns[i].point;
                std::cout<<"("<<turn.lon_<<", "<<turn.lat_<<");";
            }
            std::cout<<std::endl;
        }

        auto& out_ring = pg.outer();
        auto& in_ring = pg.inners();
        uint64_t passed_next;
        //id - это индекс пересечения
        for(uint64_t id=0;id<turns.size();++id){
            uint64_t current_id = id%turns.size();
            //индекс сегмента совпадает с индексом вершины
            auto first_point = turns[current_id].operations[1].seg_id.segment_index;
            //индекс сегмента на 1 больше индекса вершины
            auto second_point = turns[(current_id+1)%turns.size()].operations[1].seg_id.segment_index;
            //вставляется точка пересечения (вход в полигон)
            new_pg.outer().push_back(turns[current_id].point);
            //следующий индекс точки пересечения
            //обходит все точки пересечения, которые пересекают полигон с другой стороны
            auto next_id = find_next(turns,current_id);
            //добавить в коллекцию пропущенные точки пересечения (сделать вектор пар)
            //вставляется следующая точка пересечения (выход из полигона)
            new_pg.outer().push_back(turns[next_id].point);
            passed_next=second_point;
            if(id!=turns.size())
                id=next_id;
            //поиск следующей точки пересечения и входа в полигон
            //cur - это текущая вершина полигона
            for(uint64_t cur=(turns[next_id].operations[1].seg_id.segment_index+1)%out_ring.size()/*+1 или -1*/;
                cur!=(turns[(next_id+1+turns.size())%turns.size()].operations[1].seg_id.segment_index+1)%out_ring.size();
                cur=(cur+1)%out_ring.size())
                    //если 
                    if(new_pg.outer().back()!=out_ring[cur])
                        new_pg.outer().push_back(out_ring[cur]);
        }
        bg::correct(new_pg);
        pg.outer().swap(new_pg.outer());
    }
    for(auto& [lat,lon]:pg.outer())
        std::cout<<"("<<lon<<","<<lat<<")";
    std::cout<<std::endl;
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
        bg::model::multi_linestring<bg::model::linestring<Coord>> polyline = {
            line_t{{.lat_=-1, .lon_=7},
            {.lat_=21, .lon_=7}}
        };
        bg::model::polygon<Coord,true> pg;
        auto& external = pg.outer();
        external.push_back({.lat_=-1,.lon_=5});
        external.push_back({.lat_=6,.lon_=5});
        external.push_back({.lat_=6,.lon_=10});
        external.push_back({.lat_=5,.lon_=10});
        external.push_back({.lat_=5,.lon_=6});
        external.push_back({.lat_=4,.lon_=6});
        external.push_back({.lat_=4,.lon_=11});
        external.push_back({.lat_=7,.lon_=11});
        external.push_back({.lat_=9,.lon_=10});
        external.push_back({.lat_=9,.lon_=6});
        external.push_back({.lat_=8,.lon_=6});
        external.push_back({.lat_=8,.lon_=9});
        external.push_back({.lat_=7,.lon_=9});
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
        external.push_back({.lat_=0,.lon_=10});
        external.push_back({.lat_=3,.lon_=10});
        external.push_back({.lat_=-1,.lon_=5});
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
        cut(polyline,pg);
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
