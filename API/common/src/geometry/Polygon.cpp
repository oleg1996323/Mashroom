#include "common/geometry/Polygon.h"

uint64_t Polygon::vertices_per_thread_ = std::max(1u,std::thread::hardware_concurrency());

void print(const std::vector<std::pair<Coord,Line::id>>& turns){
    uint64_t i = 0;       
    for(auto& turn:turns){
        std::cout<<i<<" point: ("<<turn.first.lon_<<","<<turn.first.lat_<<")"<<std::endl;
        std::cout<<"polyline segment: "<<turn.second<<std::endl;
        std::cout<<"polygon segment: "<<turn.second<<std::endl;
        ++i;
    }
    std::cout<<std::endl;
}
void print(const Coord& point,std::string_view desc){
    if(!desc.empty())
        std::cout<<desc.data()<<": ("<<point.lon_<< \
                ", "<<point.lat_<<")"<<std::endl;
    else std::cout<<"("<<point.lon_<< \
                ", "<<point.lat_<<")"<<std::endl;
}
void print(const Polygon& pg){
    std::cout<<"outer: ";
    if(!pg.vertices().empty()){
        for(auto& vertice:pg.vertices())
            print(vertice);
    }
    else std::cout<<"NaN";
    std::cout<<std::endl;
}
bool Polygon::point_inside(const Coord& point,
        uint32_t max_threads) const noexcept{
    if(vertices_.size()<3)
        return false;
    else if(vertices_.size()==2 &&
        std::abs(std::abs(vertices_[0].lat_)-90.)<
            std::numeric_limits<Lat>::epsilon()&&
        std::abs(std::abs(vertices_[1].lat_)-90.)<
            std::numeric_limits<Lat>::epsilon()&&
        vertices_[1].lat_!=vertices_[0].lat_)
            return true;
    else{
        auto process = [point](std::span<const Coord> subsequence) noexcept
        {
            uint64_t count = 0;
            for(uint64_t i=0;i<subsequence.size();++i){
                if(Line line = Line(
                    subsequence[(i+1)%subsequence.size()].lon_,
                    subsequence[(i+1)%subsequence.size()].lat_,
                    subsequence[i].lon_,
                    subsequence[i].lat_);std::isfinite(line.k()))
                {
                    if(std::abs(line.k())<std::numeric_limits<double>::epsilon())
                        continue;
                    else if(line.k()>0)
                        if(Line::straight_not_above_the_point(
                            line,
                            point.lon_,
                            point.lat_
                        ))
                            ++count;
                    else{
                        if(Line::straight_not_under_the_point(
                            line,
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
        threads = std::min(threads,max_threads);
        vertices_processed = std::max(vertices_.size()/threads,vertices_processed);
        std::vector<Coord> last{vertices_.back(),vertices_.front()};
        if(threads>1){
            std::vector<std::future<uint64_t>> processes;
            processes.reserve(threads);
            for(uint32_t i=0;i<vertices_.size();i+=vertices_processed){
                const auto chunk_end = std::min(vertices_.begin() + i + vertices_processed, vertices_.end());
                processes.push_back(std::async(std::launch::async,
                        process,
                        std::span<const Coord>(
                                            vertices_.begin()+i,
                                            chunk_end)));
            }
            return ((std::accumulate(processes.begin(),processes.end(),0,
            [](uint64_t sum,std::future<uint64_t>& process_result)->uint64_t
            {
                return sum+process_result.get();
            })+process(std::span<const Coord>(last)))%2)==1;
        }
        else return ((process(std::span(vertices_))+
                process(std::span<const Coord>(last)))%2)==1;
    }
}
std::vector<Polygon> Polygon::intersection(const Polygon& other) const noexcept{
    if(!is_valid() || !other.is_valid())
        return std::vector<Polygon>();
    else{
        double X1 = std::max(vertices_[minX_.front()].lon_,vertices_[other.minX_.front()].lon_);
        double Y1 = std::max(vertices_[minY_.front()].lat_,vertices_[other.minY_.front()].lat_);
        double X2 = std::min(vertices_[maxX_.front()].lon_,vertices_[other.maxX_.front()].lon_);
        double Y2 = std::min(vertices_[maxY_.front()].lat_,vertices_[other.maxY_.front()].lat_);
        std::array<Line,4> lines{
            Line(X1,Y1,X1,Y2),
            Line(X1,Y2,X2,Y2),
            Line(X2,Y2,X2,Y1),
            Line(X2,Y1,X1,Y1)};
        std::vector<Polygon> pgs = this->cut(lines[0],false);
        for(uint8_t i = 1;i<lines.size();++i){
            std::vector<Polygon> pgs_tmp;
            for(uint64_t j = 0;j<pgs.size();++j){
                const Polygon& subpg = pgs[j];
                std::vector<Polygon> current_pgs_tmp = subpg.cut(lines[i],false);
                pgs_tmp.reserve(pgs_tmp.size()+current_pgs_tmp.size());
                for(const Polygon& pg_tmp:current_pgs_tmp)
                    pgs_tmp.push_back(pg_tmp);
            }
            pgs.swap(pgs_tmp);
        }
        return pgs;
    }
}
std::vector<Polygon> Polygon::cut(
        const Line& line,
        const Polygon& pg,
        bool left_forward
        ) noexcept
{
    std::vector<Polygon> result;
    auto first = line.first();
    auto second = line.second();
    bool dy_positive = first.lat_<second.lat_;
    bool dy_zero = first.lat_==second.lat_;
    bool dx_positive = first.lon_<second.lon_;
    bool dx_zero = first.lon_==second.lon_;
    Line seg;
    if(dy_positive)
        seg = Line(first.lon_,first.lat_,second.lon_,second.lat_);
    else{
        if(dy_zero && dx_positive)
            seg = Line(first.lon_,first.lat_,second.lon_,second.lat_);
        else{
            seg = Line(second.lon_,second.lat_,first.lon_,first.lat_);
            left_forward = !left_forward;
        }   
    }
    return pg.cut_impl(seg,left_forward,pg.is_clockwise());
}
std::vector<Polygon> Polygon::cut(
        const Line& line,
        bool left_forward
        ) const noexcept
{
    return cut(line,*this,left_forward);
}

std::vector<Polygon> Polygon::cut_impl(
    const Line& segment,
    bool left_forward,
    bool clockwise
    ) const noexcept{
    std::vector<Polygon> result;
    std::vector<std::pair<Coord,Line::id>> turns = line_intersections(segment);
    //print(turns);
    std::vector<std::pair<Coord,Line::id>> by_coord_turns(turns);
    std::function<bool(const std::pair<Coord,Line::id>&,const std::pair<Coord,Line::id>&)> comp;
    if(clockwise)
        comp = [&left_forward](const std::pair<Coord,Line::id>& lhs,const std::pair<Coord,Line::id>& rhs){
            if(!left_forward){
                if(std::abs(lhs.first.lat_-rhs.first.lat_)< \
                    std::numeric_limits<Lat>::epsilon())
                    return (lhs.first.lon_<rhs.first.lon_);
                else return lhs.first.lat_<rhs.first.lat_;
            }
            else{
                if(std::abs(lhs.first.lat_-rhs.first.lat_)< \
                    std::numeric_limits<Lat>::epsilon())
                    return (lhs.first.lon_>rhs.first.lon_);
                else return lhs.first.lat_>rhs.first.lat_;
            }
        };
    else{
        comp = [&left_forward](const std::pair<Coord,Line::id>& lhs,const std::pair<Coord,Line::id>& rhs){
            if(!left_forward){
                if(std::abs(lhs.first.lat_-rhs.first.lat_)< \
                    std::numeric_limits<Lat>::epsilon())
                    return (lhs.first.lon_>rhs.first.lon_);
                else return lhs.first.lat_>rhs.first.lat_;
            }
            else{
                if(std::abs(lhs.first.lat_-rhs.first.lat_)< \
                    std::numeric_limits<Lat>::epsilon())
                    return (lhs.first.lon_<rhs.first.lon_);
                else return lhs.first.lat_<rhs.first.lat_;
            }
        };
    }
    std::sort(by_coord_turns.begin(),by_coord_turns.end(),comp);
    std::vector<std::vector<uint64_t>> pgs_turns;
    std::vector<std::vector<uint64_t>> pgs_turns_in_process;
    pgs_turns_in_process.reserve(5);
    int8_t inc = 1;//increment<CLOCKWISE>(polyline[first],polyline[second],left_forward);
    int8_t point_id = 1;
    // if(!left_forward)
    //     point_id = 1;
    // else
    //     point_id = 0;
    print(by_coord_turns);
    for(uint64_t first = 0, second=first+1;first<turns.size();
        first+=2,second+=2)
    {
        uint64_t turn_id_1 = define_turn_id(turns,by_coord_turns[first]);
        uint64_t turn_id_2 = define_turn_id(turns,by_coord_turns[second]);
        const std::pair<Coord,Line::id>& turn_1 = turns[turn_id_1];
        const std::pair<Coord,Line::id>& turn_2 = turns[turn_id_2];
        if((turn_id_2+inc+turns.size())%turns.size()==turn_id_1){
            std::cout<<"new polygon detected"<<std::endl;
            pgs_turns.emplace_back().push_back(turn_id_1);
            pgs_turns.back().push_back(turn_id_2);
            print(turn_1.first,"inserted openning point");
            print(turn_2.first,"inserted closing point");
            std::cout<<"polygon completed"<<std::endl;
        }
        else if(!pgs_turns_in_process.empty() && !pgs_turns_in_process.back().empty()){
            if((turn_id_2+inc+turns.size())%turns.size()==pgs_turns_in_process.back().front())
            {
                pgs_turns.emplace_back(pgs_turns_in_process.back());
                pgs_turns.back().push_back(turn_id_1);
                pgs_turns.back().push_back(turn_id_2);
                print(turn_1.first,"inserted openning point");
                print(turn_2.first,"inserted closing point");
                pgs_turns_in_process.pop_back();
                std::cout<<"polygon completed"<<std::endl;
            }
            else if( pgs_turns_in_process.back().back()==(turn_id_1-1+turns.size())%turns.size()){
                pgs_turns_in_process.back().push_back(turn_id_1);
                pgs_turns_in_process.back().push_back(turn_id_2);
                print(turn_1.first,"inserted openning point");
                print(turn_2.first,"inserted closing point");
            }
            else{
                std::cout<<"new polygon detected"<<std::endl;
                pgs_turns_in_process.emplace_back().push_back(turn_id_1);
                pgs_turns_in_process.back().push_back(turn_id_2);
                print(turn_1.first,"inserted openning point");
                print(turn_2.first,"inserted closing point");
            }
        }
        else{
            std::cout<<"new polygon detected"<<std::endl;
            pgs_turns_in_process.emplace_back().push_back(turn_id_1);
            pgs_turns_in_process.back().push_back(turn_id_2);
            print(turn_1.first,"inserted openning point");
            print(turn_2.first,"inserted closing point");
        }
        //искать пары и записывать для каждого полигона
        //затем линейно пройтись first до second по вершинам сегментов и составить последовательно полигоны
    }
    
    for(const auto& pg_turns:pgs_turns){
        std::cout<<"new polygon"<<std::endl;
        Polygon& new_pg = result.emplace_back();
        for(uint64_t i=0;i<pg_turns.size();++i){
            //открывающий
            if(i%2==0){
                print(turns[pg_turns[i]].first,"inserted vertice");
                new_pg.append(turns[pg_turns[i]].first);
            }
            else{
                print(turns[pg_turns[i]].first,"inserted vertice");
                new_pg.append(turns[pg_turns[i]].first);
                uint64_t this_seg = (turns[pg_turns[i]].second+point_id)%number_vertices();
                uint64_t next_i = (i+1+pg_turns.size())%pg_turns.size();
                uint64_t next_seg = (turns[pg_turns[next_i]].\
                        second+point_id)%number_vertices();
                for(uint64_t j = this_seg;
                    j!=next_seg;
                    j=(j+inc+this->number_vertices())%this->number_vertices())
                {
                    if(this->vertices().at(j)!=new_pg.vertices_.back()){
                        print(this->vertices().at(j),"inserted vertice");
                        new_pg.append(this->vertices().at(j));
                    }
                }
            }
        }
    }
    return result;
}