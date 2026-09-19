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

bg::model::multi_polygon<bg::model::polygon<Coord>> cut(const bg::model::linestring<Coord>& polyline,
    const bg::model::polygon<Coord,true>& pg,
    bool left_forward //оставляем слева и сверху по ходу полилинии (либо квадранты?)
    ){
    bg::model::multi_polygon<bg::model::polygon<Coord>> result;
    for(auto seg_pl = bg::segments_begin(polyline);
         seg_pl != bg::segments_end(polyline); ++seg_pl){
        bg::model::polygon<Coord,true> new_pg;
        typedef bg::detail::overlay::turn_info<Coord> turn_info;
        std::vector<turn_info> turns;
        bg::detail::self_get_turn_points::no_interrupt_policy policy;
        bg::model::linestring<Coord> seg_ls;
        seg_ls.push_back(*seg_pl->first);
        seg_ls.push_back(*seg_pl->second);
        bg::get_turns<false,false,bg::detail::overlay::assign_null_policy>(
            seg_ls,
            pg,bg::strategies::relate::services::default_strategy<bg::model::linestring<Coord>,
                bg::model::polygon<Coord,true>>::type(),turns,policy);
        std::vector<turn_info> by_coord_turns(turns);
        std::sort(by_coord_turns.begin(),by_coord_turns.end(),
            [](const turn_info& lhs,const turn_info& rhs)
            {
                return lhs.point.lat_>rhs.point.lat_; 
            });
        
        for(auto& turn:turns){
            std::cout<<"point: ("<<turn.point.lon_<<","<<turn.point.lat_<<")"<<std::endl;
            std::cout<<"polyline segment: "<<turn.operations[0].seg_id.segment_index<<std::endl;
            std::cout<<"polygon segment: "<<turn.operations[1].seg_id.segment_index<<std::endl;
        }
        std::cout<<std::endl;
        uint64_t turns_offset = std::max_element(turns.begin(),turns.end(),
            [](const turn_info& lhs,const turn_info& rhs)
        {
            return lhs.point.lat_<
                rhs.point.lat_;
        })-turns.begin();
        uint64_t turn_id = turns_offset;
        //на случай, если несколько точек пересечения в одной координате
        uint64_t current_oppening_id= turn_id;
        std::vector<uint64_t> passed;
        bool dy_positive = seg_pl->first->lat_<seg_pl->second->lat_;
        bool dy_zero = seg_pl->first->lat_==seg_pl->second->lat_;
        bool dx_positive = seg_pl->first->lon_<seg_pl->second->lon_;
        bool dx_zero = seg_pl->first->lon_==seg_pl->second->lon_;
        int8_t inc;
        int8_t point_id = left_forward?0:1;
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
        while(true){
            auto& openning_point = turns[current_oppening_id].point;
            std::cout<<"openning point: ("<<openning_point.lon_<< \
                ", "<<openning_point.lat_<<");"<<std::endl;
            new_pg.outer().push_back(openning_point);
            auto next_by_coord = (std::lower_bound(by_coord_turns.begin(),by_coord_turns.end(),openning_point,
                [](const turn_info& t,const Coord& point)
            {
                if(point.lat_==t.point.lat_)
                    return t.point.lon_>point.lon_;
                return t.point.lat_>point.lat_;
            })-by_coord_turns.begin()+1)%by_coord_turns.size();
            uint64_t closing_by_coord = next_by_coord;
            uint64_t closing_id = std::lower_bound(turns.begin(),turns.end(),by_coord_turns[next_by_coord],
                [](const turn_info& t,const turn_info& turn)
            {
                return t.operations[1].seg_id.segment_index<turn.operations[1].seg_id.segment_index;
            }) - turns.begin();
            auto& closing_point = turns[closing_id].point;
            new_pg.outer().push_back(closing_point);
            std::cout<<"closing point: ("<<closing_point.lon_<< \
                ", "<<closing_point.lat_<<");"<<std::endl;
            std::cout<<"Search openning"<<std::endl;
            //берем индекс сегмента закрывающего пересечения
            uint64_t search_open_id = turns[closing_id].operations[1].seg_id.segment_index;            
            while(true){
                search_open_id = (search_open_id+inc+pg.outer().size())%pg.outer().size();
                uint64_t index_point = (search_open_id+point_id)%pg.outer().size();
                //ищем, есть ли данный индекс сегмента в пересечениях
                auto found_segment = std::lower_bound(turns.begin(),turns.end(),search_open_id,
                    [](const turn_info& t,uint64_t id)
                {
                    return t.operations[1].seg_id.segment_index<id;
                });
                //если есть и точно соответствует
                if( found_segment!=turns.end()
                    && found_segment->operations[1].seg_id.segment_index==search_open_id){
                    uint64_t found_id = static_cast<uint64_t>(found_segment-turns.begin());
                    //если точка не равна замыкающей точке
                    if(pg.outer().at(index_point)!=closing_point){
                        std::cout<<"Inserted point:"<<std::endl;
                        std::cout<<"("<<pg.outer().at(index_point).lon_<<", "<< \
                        pg.outer().at(index_point).lat_<<");"<<std::endl;
                        new_pg.outer().push_back(pg.outer().at(index_point));
                    }
                    //вернулись на начальную точку (проверить, не меняется ли turn_id в верхем цикле)
                    if(turn_id == found_id){
                        std::cout<<"Current state:"<<std::endl;
                        for(auto& vertice:new_pg.outer()){
                            std::cout<<"("<<vertice.lon_<<", "<<vertice.lat_<<");";
                        }
                        std::cout<<std::endl;
                        //замыкаем полигон
                        new_pg.outer().push_back(turns[turn_id].point);
                        //вставляем полигон в пул полигонов
                        result.push_back(std::move(new_pg));
                        //откуда next_by_coord
                        turn_id = std::lower_bound(turns.begin(),turns.end(),by_coord_turns[next_by_coord],
                            [](const turn_info& t,const turn_info& turn)
                        {
                            return t.operations[1].seg_id.segment_index<turn.operations[1].seg_id.segment_index;
                        }) - turns.begin();
                        current_oppening_id = turn_id;
                        //если вернулись на самую верхнюю точку - обрезка завершена. Возвращаем пул полигонов
                        if(turn_id == turns_offset)
                            if(passed.empty())
                                return result; //вернуть пул полигонов. Изменить сигнатуру функции
                            else{
                                search_open_id = passed.back();
                                passed.pop_back();
                                break;
                            }
                        break;
                    }
                    //если не замкнули полигон
                    else{
                        //проверить входит ли эта точка в passed. Если да, то исключить
                        //не добавлять те точки, которые были обработаны сначала


                        //ищем позицию найденного пересечения по координатам
                        uint64_t found_by_coord = std::lower_bound(
                            by_coord_turns.begin(),
                            by_coord_turns.end(),
                            found_segment->point,
                            [](const turn_info& t,const Coord& point)
                            {
                                if(point.lat_==t.point.lat_)
                                    return t.point.lon_>point.lon_;
                                return t.point.lat_>point.lat_;
                            })-by_coord_turns.begin();
                        //если между закрывающим пересечением и найденным открывающим пересечением
                        //есть еще одно пересечение, то регистрируем его
                        if(found_by_coord-closing_by_coord>1){
                            uint64_t after_closing_passed = std::lower_bound(turns.begin(),turns.end(),
                                by_coord_turns[(found_by_coord+1)%by_coord_turns.size()],
                                [](const turn_info& t,const turn_info& turn){
                                    if(turn.point.lat_==t.point.lat_)
                                        return t.point.lon_>turn.point.lon_;
                                    return t.point.lat_>turn.point.lat_;
                                })-turns.begin();
                            std::cout<<"Found passed point between: ("<< \
                                found_segment->point.lon_<<", "<<found_segment->point.lat_<<");("<< \
                                closing_point.lon_<<", "<<closing_point.lat_<<")"<<std::endl;
                            auto cont = std::span(by_coord_turns.cbegin()+closing_by_coord+1,by_coord_turns.cbegin()+found_by_coord);
                            for(uint64_t i = found_by_coord-1;
                                    i>closing_by_coord;
                                    --i)
                                passed.push_back(i);
                            std::cout<<"Passed points:"<<std::endl;
                            for(auto& id:std::span(passed.begin(),passed.end())){
                                std::cout<<"("<<by_coord_turns[id].point.lon_<<","<<by_coord_turns[id].point.lat_<<")";
                            }
                            std::cout<<std::endl;
                        }
                        current_oppening_id = found_id;
                        break;
                    }
                }
                //это просто вершина
                else{
                    if(pg.outer().at(index_point)!=closing_point){
                        std::cout<<"Inserted point:"<<std::endl;
                        std::cout<<"("<<pg.outer().at(index_point).lon_<<", "<< \
                        pg.outer().at(index_point).lat_<<");"<<std::endl;
                        new_pg.outer().push_back(pg.outer().at(index_point));
                    }
                }                
            }
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
